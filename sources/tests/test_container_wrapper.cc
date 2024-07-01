#include "tests/test_container_wrapper.h"

#include <algorithm>
#include <random>

namespace Test
{

#ifndef TEST_MATERIALS_PATH
#define TEST_MATERIALS_PATH "."
#endif

std::string GetTestFilePath(const std::string& relative_path)
{
    return std::string(TEST_MATERIALS_PATH) + "/" + relative_path;
}

std::string demangle(const char* mangled_name)
{
    int status = 0;
    char* demangled = abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status);
    std::string demangled_name = (status == 0) ? demangled : mangled_name;
    std::free(demangled);
    return demangled_name;
}

#define EXPECT_EXCEPTION(statement, expected_exception, expected_message) \
    try \
    { \
        statement; \
    } \
    catch (const expected_exception& e) \
    { \
        if (std::string(e.what()) != (expected_message)) \
        { \
            FAIL() << "Expected message: \"" << (expected_message) << "\", but got: \"" << e.what() << "\""; \
        } \
    } \
    catch (...) \
    { \
        FAIL() << "Expected exception of type " << demangle(typeid(expected_exception).name()) << " but got a different exception."; \
    }

template<class Container>
void InsertKeys(ContainerWrapper<Container>* container_wrapper,
                const std::vector<std::pair<typename Container::key_type, Value>>& keys,
                const bool expect)
{
    for (const auto& kv : keys)
    {
        EXPECT_EQ(container_wrapper->Insert(kv.first, kv.second, 0), expect);
    }
}

template<class Container>
void EraseKeys(ContainerWrapper<Container>* container_wrapper,
               const std::vector<std::pair<typename Container::key_type, Value>>& keys,
               const bool expect)
{
    for (const auto& kv : keys)
    {
        EXPECT_EQ(container_wrapper->Erase(kv.first), expect);
    }
}

template<class Container>
void CompareFileAndContainerContents(ContainerWrapper<Container>* container_wrapper,
                                     const std::filesystem::path& path,
                                     std::map<std::string, std::size_t>* life_time_list)
{
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        typename Container::key_type key;
        typename Container::mapped_type value;
        std::size_t life_time = 0;

        try
        {
            iss >> key >> value;
            if (!iss.fail() && !iss.eof() && life_time_list)
            {
                iss >> life_time;
                EXPECT_NEAR(container_wrapper->TTL(key), life_time_list->at(key), 1);
            }
            EXPECT_EQ(container_wrapper->GetValue(key), value);
        }
        catch (const std::exception& e)
        {
            continue;
        }
    }
}

template<class Container>
void CheckUpload(ContainerWrapper<Container>* container_wrapper,
                 const std::filesystem::path& path,
                 std::size_t expected_num_entries,
                 std::map<std::string, std::size_t>* life_time_list = nullptr)
{
    EXPECT_EQ(container_wrapper->Upload(path), expected_num_entries);
    if (expected_num_entries != 0)
    {
        CompareFileAndContainerContents(container_wrapper, path, life_time_list);
    }
}

template<class Container>
void CheckExport(ContainerWrapper<Container>* container_wrapper,
                 const std::filesystem::path& input_path,
                 const std::filesystem::path& output_path,
                 std::size_t expected_num_entries,
                 std::map<std::string, std::size_t>* life_time_list = nullptr)
{
    CheckUpload(container_wrapper, input_path, expected_num_entries, life_time_list);
    EXPECT_EQ(container_wrapper->Export(output_path), expected_num_entries);
    CompareFileAndContainerContents(container_wrapper, output_path, life_time_list);
}

TYPED_TEST(ContainerWrapperSuite, Insert_DifferentKeys)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
}

TYPED_TEST(ContainerWrapperSuite, Insert_IdenticalKeys)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, false);
}

TYPED_TEST(ContainerWrapperSuite, Insert_IdenticalKeys_Timestamp)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    EXPECT_FALSE(this->container_wrapper->Insert("any_key", value1, 0));
}

TYPED_TEST(ContainerWrapperSuite, Insert_Timestamp)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    sleep(1);
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 0));
}

TYPED_TEST(ContainerWrapperSuite, GetValue_KeyExists)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    for (const auto& kv : this->params_.std_dataset_identical_values)
    {
        EXPECT_EQ(this->container_wrapper->GetValue(kv.first), value1);
    }
}

TYPED_TEST(ContainerWrapperSuite, GetValue_KeyDoesNotExist)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    EXPECT_ANY_THROW(this->container_wrapper->GetValue("any_key"));
}

TYPED_TEST(ContainerWrapperSuite, GetValue_ContainerEmpty)
{
    EXPECT_ANY_THROW(this->container_wrapper->GetValue("any_key"));
}

TYPED_TEST(ContainerWrapperSuite, GetValue_Timestamp)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    EXPECT_EQ(this->container_wrapper->GetValue("any_key"), value1);
    sleep(1);
    EXPECT_ANY_THROW(this->container_wrapper->GetValue("any_key"));
}

TYPED_TEST(ContainerWrapperSuite, Exists_KeyExists)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    for (const auto& kv : this->params_.std_dataset_identical_values)
    {
        EXPECT_TRUE(this->container_wrapper->Exists(kv.first));
    }
}

TYPED_TEST(ContainerWrapperSuite, Exists_KeyDoesNotExist)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    EXPECT_FALSE(this->container_wrapper->Exists("any_key"));
}

TYPED_TEST(ContainerWrapperSuite, Exists_ContainerEmpty)
{
    EXPECT_FALSE(this->container_wrapper->Exists("any_key"));
}

TYPED_TEST(ContainerWrapperSuite, Exists_Timestamp)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    EXPECT_TRUE(this->container_wrapper->Exists("any_key"));
    sleep(1);
    EXPECT_FALSE(this->container_wrapper->Exists("any_key"));
}

TYPED_TEST(ContainerWrapperSuite, Erase_KeyExists)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    EraseKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
}

TYPED_TEST(ContainerWrapperSuite, Erase_KeyDoesNotExist)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    EXPECT_FALSE(this->container_wrapper->Erase("any_key"));
}

TYPED_TEST(ContainerWrapperSuite, Erase_ContainerEmpty)
{
    EXPECT_FALSE(this->container_wrapper->Erase("any_key"));
}

TYPED_TEST(ContainerWrapperSuite, Erase_AllElements_RandomOrder)
{
    for (auto i = 0; i < 100; ++i)
    {
        InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
        auto shuffled_dataset = this->params_.std_dataset_identical_values;
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(shuffled_dataset.begin(), shuffled_dataset.end(), g);
        EraseKeys(this->container_wrapper, shuffled_dataset, true);
    }
}

TYPED_TEST(ContainerWrapperSuite, Erase_TimeExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    sleep(1);
    EXPECT_FALSE(this->container_wrapper->Erase("any_key"));
}

TYPED_TEST(ContainerWrapperSuite, Erase_TimeNotExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    EXPECT_TRUE(this->container_wrapper->Erase("any_key"));
}

TYPED_TEST(ContainerWrapperSuite, Update_AllValueFields)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    for (const auto& kv : this->params_.std_dataset_identical_values)
    {
        EXPECT_EQ(this->container_wrapper->GetValue(kv.first), value1);
    }
    for (const auto& kv : this->params_.std_dataset_identical_values)
    {
        EXPECT_TRUE(this->container_wrapper->Update(kv.first, value2));
    }
    for (const auto& kv : this->params_.std_dataset_identical_values)
    {
        EXPECT_EQ(this->container_wrapper->GetValue(kv.first), value2);
    }
}

TYPED_TEST(ContainerWrapperSuite, Update_NoOneValueField)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 0));
    EXPECT_FALSE(this->container_wrapper->Update("any_key", Value{}));
    EXPECT_EQ(this->container_wrapper->GetValue("any_key"), value1);
}

TYPED_TEST(ContainerWrapperSuite, Update_1_3_5)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 0));
    EXPECT_TRUE(this->container_wrapper->Update("any_key", Value{"abcdef", "-", 1900, "-", 99}));
    Value result = {"abcdef", "first_name", 1900, "city", 99};
    EXPECT_EQ(this->container_wrapper->GetValue("any_key"), result);
}

TYPED_TEST(ContainerWrapperSuite, Update_TimeExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    sleep(1);
    EXPECT_FALSE(this->container_wrapper->Update("any_key", value2));
}

TYPED_TEST(ContainerWrapperSuite, Update_TimeNotExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    EXPECT_TRUE(this->container_wrapper->Update("any_key", value2));
}

TYPED_TEST(ContainerWrapperSuite, Keys)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    std::vector<std::string> expected_result;
    for (const auto& kv : this->params_.std_dataset_identical_values)
    {
        expected_result.push_back(kv.first);
    }
    auto result = this->container_wrapper->Keys();
    std::sort(result.begin(), result.end());
    std::sort(expected_result.begin(), expected_result.end());
    EXPECT_EQ(result, expected_result);
}

TYPED_TEST(ContainerWrapperSuite, Keys_ContainerEmpty)
{
    std::vector<std::string> result{};
    EXPECT_EQ(this->container_wrapper->Keys(), result);
}

TYPED_TEST(ContainerWrapperSuite, Keys_Timestamp)
{
    EXPECT_TRUE(this->container_wrapper->Insert("key1", value1, 1));
    EXPECT_TRUE(this->container_wrapper->Insert("key2", value1, 2));
    EXPECT_TRUE(this->container_wrapper->Insert("key3", value1, 1));
    EXPECT_TRUE(this->container_wrapper->Insert("key4", value1, 2));
    EXPECT_TRUE(this->container_wrapper->Insert("key5", value1, 1));

    std::vector<std::string> expected_result{"key1", "key2", "key3", "key4", "key5"};
    auto result = this->container_wrapper->Keys();
    std::sort(result.begin(), result.end());
    EXPECT_EQ(result, expected_result);

    sleep(1);
    expected_result = {"key2", "key4"};
    result = this->container_wrapper->Keys();
    std::sort(result.begin(), result.end());
    EXPECT_EQ(result, expected_result);

    sleep(1);
    expected_result = {};
    result = this->container_wrapper->Keys();
    std::sort(result.begin(), result.end());
    EXPECT_EQ(result, expected_result);
}

TYPED_TEST(ContainerWrapperSuite, Rename)
{
    EXPECT_TRUE(this->container_wrapper->Insert("key1", value1, 0));
    EXPECT_TRUE(this->container_wrapper->Rename("key1", "key2"));
    EXPECT_TRUE(this->container_wrapper->Exists("key2"));
    EXPECT_FALSE(this->container_wrapper->Exists("key1"));
    EXPECT_EQ(this->container_wrapper->GetValue("key2"), value1);
}

TYPED_TEST(ContainerWrapperSuite, Rename_CurrentKeyDoesNotExist)
{
    EXPECT_FALSE(this->container_wrapper->Rename("key1", "key2"));
}

TYPED_TEST(ContainerWrapperSuite, Rename_NewKeyExists)
{
    EXPECT_TRUE(this->container_wrapper->Insert("key1", value1, 0));
    EXPECT_TRUE(this->container_wrapper->Insert("key2", value1, 0));
    EXPECT_FALSE(this->container_wrapper->Rename("key1", "key2"));
}

TYPED_TEST(ContainerWrapperSuite, Rename_CurrentKey_TimeExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("key1", value1, 1));
    sleep(1);
    EXPECT_FALSE(this->container_wrapper->Rename("key1", "key2"));
}

TYPED_TEST(ContainerWrapperSuite, Rename_CurrentKey_TimeNotExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("key1", value1, 1));
    EXPECT_TRUE(this->container_wrapper->Rename("key1", "key2"));
}

TYPED_TEST(ContainerWrapperSuite, Rename_NewKey_TimeExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("key1", value1, 0));
    EXPECT_TRUE(this->container_wrapper->Insert("key2", value1, 1));
    sleep(1);
    EXPECT_TRUE(this->container_wrapper->Rename("key1", "key2"));
}

TYPED_TEST(ContainerWrapperSuite, Rename_NewKey_TimeNotExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("key1", value1, 0));
    EXPECT_TRUE(this->container_wrapper->Insert("key2", value1, 1));
    EXPECT_FALSE(this->container_wrapper->Rename("key1", "key2"));
}

TYPED_TEST(ContainerWrapperSuite, TTL_KeyWithoutExpirationDate)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 0));
    EXPECT_EXCEPTION(this->container_wrapper->TTL("any_key"), std::runtime_error, "The key does not have a timestamp.");
}

TYPED_TEST(ContainerWrapperSuite, TTL_KeyWithExpirationDate)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 3));
    EXPECT_NEAR(this->container_wrapper->TTL("any_key"), 3, 1);
    sleep(2);
    EXPECT_NEAR(this->container_wrapper->TTL("any_key"), 1, 1);
    sleep(1);
    EXPECT_EXCEPTION(this->container_wrapper->TTL("any_key"), std::runtime_error, "The key was not found.");
}

TYPED_TEST(ContainerWrapperSuite, TTL_KeyDoesNotExist)
{
    EXPECT_EXCEPTION(this->container_wrapper->TTL("any_key"), std::runtime_error, "The key was not found.");
}

TYPED_TEST(ContainerWrapperSuite, Find_AllValueFields)
{
    EXPECT_TRUE(this->container_wrapper->Insert("key1", value1, 0));
    EXPECT_TRUE(this->container_wrapper->Insert("key2", value2, 0));
    EXPECT_TRUE(this->container_wrapper->Insert("key3", value1, 0));
    std::vector<std::string> result = {"key1", "key3"};
    EXPECT_EQ(this->container_wrapper->Find(value1), result);
}

TYPED_TEST(ContainerWrapperSuite, Find_NoOneValueField)
{
    EXPECT_TRUE(this->container_wrapper->Insert("key1", value1, 0));
    EXPECT_TRUE(this->container_wrapper->Insert("key2", value2, 0));
    EXPECT_TRUE(this->container_wrapper->Insert("key3", value1, 0));
    std::vector<std::string> result = {"key1", "key2", "key3"};
    EXPECT_EQ(this->container_wrapper->Find(Value{}), result);
}

TYPED_TEST(ContainerWrapperSuite, Find_1_3_5)
{
    Value value3 = {"last_name", "random", 2000, "random", 183};
    EXPECT_TRUE(this->container_wrapper->Insert("key", value1, 0));
    EXPECT_TRUE(this->container_wrapper->Insert("key2", value2, 0));
    EXPECT_TRUE(this->container_wrapper->Insert("key3", value1, 0));
    EXPECT_TRUE(this->container_wrapper->Insert("key4", value3, 0));
    Value value4 = {"last_name", "-", 2000, "-", 183};
    std::vector<std::string> result = {"key", "key3", "key4"};
    EXPECT_EQ(this->container_wrapper->Find(value4), result);
}

TYPED_TEST(ContainerWrapperSuite, Find_TimeExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    sleep(1);
    EXPECT_EQ(this->container_wrapper->Find(value1), std::vector<std::string>{});
}

TYPED_TEST(ContainerWrapperSuite, Find_TimeNotExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    std::vector<std::string> result{"any_key"};
    EXPECT_EQ(this->container_wrapper->Find(value1), result);
}

TYPED_TEST(ContainerWrapperSuite, ShowAll_IdenticalValues)
{
    InsertKeys(this->container_wrapper, this->params_.std_dataset_identical_values, true);
    std::vector<Value> result{20, value1};
    EXPECT_EQ(this->container_wrapper->ShowAll(), result);
}

TYPED_TEST(ContainerWrapperSuite, ShowAll_ContainerEmpty)
{
    EXPECT_EQ(this->container_wrapper->ShowAll(), std::vector<Value>{});
}

TYPED_TEST(ContainerWrapperSuite, ShowAll_TimeExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    sleep(1);
    EXPECT_EQ(this->container_wrapper->ShowAll(), std::vector<Value>{});
}

TYPED_TEST(ContainerWrapperSuite, ShowAll_TimeNotExpired)
{
    EXPECT_TRUE(this->container_wrapper->Insert("any_key", value1, 1));
    std::vector<Value> result{value1};
    EXPECT_EQ(this->container_wrapper->ShowAll(), result);
}

TYPED_TEST(ContainerWrapperSuite, Upload_Timestamp)
{
    auto path = GetTestFilePath("upload_timestamp.txt");
    std::map<std::string, std::size_t> life_time_list = {
            {"snail", 1},
            {"youth", 2},
            {"lily", 1},
            {"tease", 3},
            {"secretion", 4},
            {"portrait", 2},
            {"cultural", 3},
            {"overcharge", 4},
            {"folklore", 3},
            {"glare", 2},
    };
    CheckUpload(this->container_wrapper, path, 10, &life_time_list);
    CheckUpload(this->container_wrapper, path, 0);
    sleep(4);
    CheckUpload(this->container_wrapper, path, 10, &life_time_list);
}

TYPED_TEST(ContainerWrapperSuite, Upload_WithoutTimestamp)
{
    auto path = GetTestFilePath("upload_without_timestamp.txt");
    CheckUpload(this->container_wrapper, path, 10);
}

TYPED_TEST(ContainerWrapperSuite, ContainerWrapperSuite)
{
    auto path = GetTestFilePath("upload_invalid_value.txt");
    CheckUpload(this->container_wrapper, path, 7);
}

TYPED_TEST(ContainerWrapperSuite, Upload_FileDoesNotExist)
{
    EXPECT_EXCEPTION(this->container_wrapper->Upload("any_file"), std::runtime_error, "File 'any_file' not exists.");
}

TYPED_TEST(ContainerWrapperSuite, Upload_FileIsDirectory)
{
    auto path = GetTestFilePath("../materials");
    EXPECT_EXCEPTION(this->container_wrapper->Upload(path), std::runtime_error, std::string("File '") + std::string{path} + std::string("' not exists."));
}

TYPED_TEST(ContainerWrapperSuite, Export_Timestamp)
{
    auto input_path = GetTestFilePath("upload_timestamp.txt");
    auto output_path = GetTestFilePath("export_timestamp_output_" + this->GetTypeName() + ".txt");
    std::map<std::string, std::size_t> life_time_list = {
            {"snail", 1},
            {"youth", 2},
            {"lily", 1},
            {"tease", 3},
            {"secretion", 4},
            {"portrait", 2},
            {"cultural", 3},
            {"overcharge", 4},
            {"folklore", 3},
            {"glare", 2},
    };
    CheckExport(this->container_wrapper, input_path, output_path, 10, &life_time_list);
    sleep(4);
    EXPECT_EQ(this->container_wrapper->Export(output_path), 0);
}

TYPED_TEST(ContainerWrapperSuite, Export_WithoutTimestamp)
{
    auto input_path = GetTestFilePath("upload_without_timestamp.txt");
    auto output_path = GetTestFilePath("export_without_timestamp_output_" + this->GetTypeName() + ".txt");
    CheckExport(this->container_wrapper, input_path, output_path, 10);
}

TYPED_TEST(ContainerWrapperSuite, Export_FileDoesNotExist)
{
    EXPECT_EXCEPTION(this->container_wrapper->Export("any_dir/"), std::runtime_error, "Unable to open the file.");
}

TYPED_TEST(ContainerWrapperSuite, Export_FileIsDirectory)
{
    auto path = GetTestFilePath("../materials/");
    EXPECT_EXCEPTION(this->container_wrapper->Export(path), std::runtime_error, path + " is directory.");
}

} // namespace Test
