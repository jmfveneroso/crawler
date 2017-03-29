#include "url_database.hpp"
#include <iostream>

namespace Crawler {

Entry::Entry() {}

Entry::Entry(bool occupied, std::string str, system_clock::time_point timestamp)
  : occupied(occupied), timestamp(timestamp) {
  str.copy(url, str.size());
}

UrlDatabase::UrlDatabase(std::shared_ptr<ILogger> logger) 
  : logger_(logger), db_file_(NULL), table_size_(0) {
  header_size_ = sizeof(size_t) * 2;
}

UrlDatabase::~UrlDatabase() {
  if (db_file_ != NULL) fclose(db_file_);
}

size_t UrlDatabase::GetHash(std::string const& s) const {
  size_t result = 2166136261U;
  std::string::const_iterator end = s.end();
  for (
    std::string::const_iterator iter = s.begin();
    iter != end;
    ++iter
  ) {
    result = (16777619 * result) ^ static_cast<unsigned char>(*iter);
  }
  return result;
}

bool UrlDatabase::Probe(size_t start, const std::string& key, Entry* entry) {
  if (fseek(db_file_, header_size_ + start * sizeof(Entry), SEEK_SET) != 0)
    throw new std::runtime_error("Seek set 1 failed.");

  while (fread(entry, sizeof(Entry), 1, db_file_) == 1) {
    if (!entry->occupied || entry->url == key) {
      return true;
    }
  }

  if (ferror(db_file_))
    throw new std::runtime_error("Error 1 reading db file.");

  // Reached end of file.
  size_t counter = 0;
  if (fseek(db_file_, header_size_, SEEK_SET) != 0)
    throw new std::runtime_error("Seek set 2 failed.");

  while (fread(entry, sizeof(Entry), 1, db_file_) == 1) {
    if (!entry->occupied || entry->url == key) {
      return true;
    }
    if (counter++ == start) break;
  }

  if (ferror(db_file_))
    throw new std::runtime_error("Error 2 reading db file.");
  
  return false;
}

bool UrlDatabase::Open(const char* filename, bool overwrite) {
  bool file_exists = access(filename, F_OK) != -1;

  size_t fingerprint = 0x462fab2668d358eb;
  table_size_ = TABLE_SIZE;
  if (!file_exists || overwrite) { 
    db_file_ = fopen(filename, "wb+");
    if (db_file_ == NULL) 
      throw new std::runtime_error("Error opening database file.");

    // Write fingerprint to the new file.
    fwrite(&fingerprint, sizeof(size_t), 1, db_file_);

    // Write table size to the new file.
    fwrite(&table_size_, sizeof(size_t), 1, db_file_);

    Entry e = { false, "\0", system_clock::time_point() };
    memset(e.url, 0, (size_t) 256);

    for (size_t i = 0; i < TABLE_SIZE; ++i) {
      fwrite(&e, sizeof(Entry), 1, db_file_);
    }
  } else { 
    db_file_ = fopen(filename, "rb+");
    if (db_file_ == NULL)
      throw new std::runtime_error("Error opening database file.");

    size_t buffer = 0;
    if (fread(&buffer, sizeof(size_t), 1, db_file_) != 1)
      throw new std::runtime_error("Error reading fingerprint.");

    // Check fingerprint.
    if (buffer != fingerprint)
      throw new std::runtime_error("The database file is invalid.");

    if (fread(&table_size_, sizeof(size_t), 1, db_file_) != 1)
      throw new std::runtime_error("Error reading table size.");
  }

  return true;
}

bool UrlDatabase::Close() {
  if (fclose(db_file_) == EOF) {
    return false;
  }
  db_file_ = NULL;
  return true;
}

bool UrlDatabase::Put(
  const std::string& key, system_clock::time_point timestamp
) {
  if (key.size() == 0 || key.size() > 256) return false;

#ifdef IN_MEMORY
  urls_[key] = timestamp; 
  return true;
#endif
  
  size_t hash = GetHash(key) % table_size_; 

  Entry entry;
  if (!Probe(hash, key, &entry)) {
    throw std::runtime_error("The hash table is full.");
  }

  // buffer_.occupied = true;
  // key.copy(buffer_.url, key.size());
  // buffer_.timestamp = timestamp;
  entry = Entry(true, key, timestamp);
  // fseeko(db_file_, -sizeof(Entry), SEEK_CUR);

  if (fseek(db_file_, -sizeof(Entry), SEEK_CUR) != 0) 
    throw new std::runtime_error("Error 3 on seek set.");

  if (fwrite(&entry, sizeof(Entry), 1, db_file_) != 1)
    throw new std::runtime_error("Error writing to db.");

  return true;
}

bool UrlDatabase::Get(const std::string& key, Entry* entry) {
#ifdef IN_MEMORY
  if (urls_.find(key) != urls_.end()) {
    entry->timestamp = urls_[key];
    return true;
  } else {
    return false;
  }
#endif

  size_t hash = GetHash(key) % table_size_; 
  if (!Probe(hash, key, entry)) {
    return false;
  }
  return entry->occupied;
}

double UrlDatabase::CheckEmptyBuckets() {
  fseeko(db_file_, header_size_, SEEK_SET);

  Entry entry;
  size_t num_occupied = 0;
  for (size_t i = 0; i < table_size_; ++i) {
    if (fread(&entry, sizeof(Entry), 1, db_file_) != 1)
      throw new std::runtime_error("Error reading entry from url database.");

    if (entry.occupied) ++num_occupied;
  }
  return (double) num_occupied / table_size_; 
}

} // End of namespace.
