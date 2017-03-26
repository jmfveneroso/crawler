#include "url_priority_list.hpp"
#include <iostream>
#include <sstream>

namespace Crawler {

UrlPriorityList::UrlPriorityList(std::shared_ptr<ILogger> logger) 
  : logger_(logger) {
  header_size_ = sizeof(size_t) * 2;
}

UrlPriorityList::~UrlPriorityList() {
}

int UrlPriorityList::GetPriority(const std::string& url) {
  size_t pos = 0, words = 0;
  while (pos != std::string::npos) {
    pos = url.find_first_of('/', pos + 1);
    ++words;
  }
  return (words > PRIORITY_LEVELS) ? (PRIORITY_LEVELS - 1) : words - 1;
}

void UrlPriorityList::CommitFileCursors() {
  for (int i = 0; i < PRIORITY_LEVELS; ++i) {
    // Write cursor to the file header.
    fseeko(priority_files_[i], sizeof(size_t), SEEK_SET);
    fwrite(&file_cursors_[i], sizeof(size_t), 1, priority_files_[i]);

    // Set cursor to read position.
    fseeko(priority_files_[i], file_cursors_[i], SEEK_SET);
  }
}

bool UrlPriorityList::Open(const std::string& file_suffix, bool overwrite) {
  // One file must be opened for each priority level.
  for (int i = 0; i < PRIORITY_LEVELS; ++i) {
    std::stringstream ss;
    ss << file_suffix << i;
    std::string filename = ss.str();

    bool file_exists = access(filename.c_str(), F_OK) != -1;
    if (!file_exists || overwrite) { 
      priority_files_[i] = fopen(filename.c_str(), "wb+");
      if (priority_files_[i] == NULL) 
        throw new std::runtime_error("Error opening priority file.");

      // Write fingerprint to the new file.
      fwrite(&fingerprint_, sizeof(size_t), 1, priority_files_[i]);

      // Write cursor to the new file.
      file_cursors_[i] = header_size_;
      fwrite(&file_cursors_[i], sizeof(size_t), 1, priority_files_[i]);
    } else {
      priority_files_[i] = fopen(filename.c_str(), "rb+");
      if (priority_files_[i] == NULL) 
        throw new std::runtime_error("Error opening priority file.");

      size_t buffer = 0;
      if (fread(&buffer, sizeof(size_t), 1, priority_files_[i]) != 1)
        throw new std::runtime_error("Error reading fingerprint.");

      // Check fingerprint.
      if (buffer != fingerprint_)
        throw new std::runtime_error("The database file is invalid.");

      if (fread(&file_cursors_[i], sizeof(size_t), 1, priority_files_[i]) != 1)
        throw new std::runtime_error("Error reading file cursor.");
      
      fseeko(priority_files_[i], 0, SEEK_END);
    }
  }
  return true;
}

bool UrlPriorityList::Close() {
  for (int i = 0; i < PRIORITY_LEVELS; ++i) {
    if (!fclose(priority_files_[i])) return false;
    priority_files_[i] = NULL;
    file_cursors_[i] = 0;
  }
  return true;
}

bool UrlPriorityList::Push(const std::string& url) {
  if (!url.size() || url.size() > 256) return false;

#ifdef IN_MEMORY
  // Write to memory.
  urls_.push(url);
  return true;
#endif

  int priority = GetPriority(url);

  unsigned char size = url.size();
  if (fwrite(&size, sizeof(unsigned char), 1, priority_files_[priority]) != 1)
    return false;

  char buffer[256];
  std::strncpy(buffer, url.c_str(), size);
  if (fwrite(buffer, sizeof(char), size, priority_files_[priority]) != size)
    return false;

  ++written_urls_num_;
  return true;
}

bool UrlPriorityList::Pop(std::string* url) {
  if (urls_.empty()) return false;

  *url = urls_.front();
  urls_.pop();
  return true;
}

size_t UrlPriorityList::FetchBlock() {
  // std::queue<std::string> empty;
  // std::swap(urls_, empty);

  // Commit the current cursor values to file and set cursors
  // to read position.
  CommitFileCursors();

  unsigned char size;
  char buffer[257];
  int priority = 0; 
  while (urls_.size() < BLOCK_SIZE) {
    if (fread(&size, sizeof(unsigned char), 1, priority_files_[priority]) != 1) {
      // Reached end of file.
      if (++priority > PRIORITY_LEVELS - 1) break;
      continue;
    }

    fread(buffer, sizeof(char), size, priority_files_[priority]);
    buffer[size] = '\0';
    urls_.push(buffer);
    file_cursors_[priority] += 1 + size;
  }

  // Set cursors back to write position.
  for (int i = 0; i < PRIORITY_LEVELS; ++i) 
    fseeko(priority_files_[i], 0, SEEK_END);

  return urls_.size();
}

} // End of namespace.
