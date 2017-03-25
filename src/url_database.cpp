#include "url_database.hpp"
#include <iostream>

namespace Crawler {

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
  fseeko(db_file_, header_size_ + start * sizeof(Entry), SEEK_SET);
  while (fread(entry, sizeof(Entry), 1, db_file_) == 1) {
    if (!entry->occupied || entry->url == key) {
      return true;
    }
  }

  // Reached end of file.
  size_t counter = 0;
  fseeko(db_file_, header_size_, SEEK_SET);
  while (fread(entry, sizeof(Entry), 1, db_file_) == 1) {
    if (!entry->occupied || entry->url == key) {
      return true;
    }
    if (counter++ == start) break;
  }
  
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

    Entry e = { false, "", system_clock::time_point() };
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
  size_t hash = GetHash(key) % table_size_; 

  Entry entry;
  if (!Probe(hash, key, &entry)) {
    throw std::runtime_error("The hash table is full.");
  }

  entry.occupied = true;
  key.copy(entry.url, 256);
  entry.timestamp = timestamp;

  fseeko(db_file_, -sizeof(Entry), SEEK_CUR);
  fwrite(&entry, sizeof(Entry), 1, db_file_);

  return true;
}

bool UrlDatabase::Get(const std::string& key, Entry* entry) {
  size_t hash = GetHash(key) % table_size_; 
  if (!Probe(hash, key, entry)) {
    return false;
  }
  return entry->occupied;
}

} // End of namespace.
