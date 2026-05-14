#include <gtest/gtest.h>

#include "cache-repository.h"
#include "types/operation-record.h"

TEST(CacheRepositoryTest, PutsAndFindsRecord) {
  Calculator::Storage::CacheRepository cache;
  Calculator::Storage::Types::OperationRecord record(1, 2, "add", 3, 0, "1+2");

  cache.put(record);

  const auto found = cache.find(2, 1, "add");

  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(found->result(), std::optional<int>{3});
  EXPECT_EQ(found->status(), 0);
}

TEST(CacheRepositoryTest, ReturnsEmptyOptionalForMissingRecord) {
  Calculator::Storage::CacheRepository cache;

  const auto found = cache.find(1, 2, "add");

  EXPECT_FALSE(found.has_value());
}

TEST(CacheRepositoryTest, WarmsUpFromRecords) {
  Calculator::Storage::CacheRepository cache;
  const std::vector<Calculator::Storage::Types::OperationRecord> records = {
      Calculator::Storage::Types::OperationRecord(2, 3, "mul", 6, 0, "2*3"),
      Calculator::Storage::Types::OperationRecord(5, std::nullopt, "fact", 120, 0, "fact(5)"),
  };

  cache.warm_up(records);

  EXPECT_EQ(cache.size(), 2U);
  EXPECT_TRUE(cache.find(3, 2, "mul").has_value());
  EXPECT_TRUE(cache.find(5, std::nullopt, "fact").has_value());
}
