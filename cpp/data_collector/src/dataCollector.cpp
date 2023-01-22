#include "dataCollector.h"

template<typename T, const DCInterface interfaceName>
class dataType{
private:
    const DCInterface mInterfaceName = interfaceName;
    mutable std::shared_mutex mMutex;
    std::vector<std::function<void()>> listOfCallbackFunctions;
    T mData;
public:
    void read(T *val){
        std::shared_lock lock(mMutex);
        *val = mData;
    }
    void write(const T *val){
        {
            std::unique_lock lock(mMutex);
            mData = *val;
        }
        // Notify registered callback post write
        for(auto const& function: listOfCallbackFunctions){
            function();
        }
    }
    void registerCallback(const std::function<void()> functionName){
        std::unique_lock lock(mMutex);
        listOfCallbackFunctions.emplace_back(functionName);
    }
};

struct mappingInfo{
    mappingInfo(std::function<void (void *data)> readFunc,
                std::function<void (const void *data)> writeFunc,
                std::function<void (const std::function<void()> functionName)> registerCallbackFunc)
                :mReadFunc(readFunc),
                 mWriteFunc(writeFunc),
                 mRegisterCallbackFunc(registerCallbackFunc){}

    std::function<void (void *data)> mReadFunc;
    std::function<void (const void *data)> mWriteFunc;
    std::function<void (const std::function<void()> functionName)> mRegisterCallbackFunc;
};

static dataType<map_transaction_product, DCInterface::transaction_product_info> tpInfo;
static dataType<map_transaction_product, DCInterface::transaction_privacy_info> tprivacyInfo;
static dataType<list_users, DCInterface::transaction_privacy_info> gUserIds;

/// @brief Mapping table to map the interface to specific details like the read/write functions
static const std::unordered_map<DCInterface, mappingInfo> mappingTable{
    { DCInterface::transaction_product_info, 
        mappingInfo(
            [](void *data){tpInfo.read(static_cast<map_transaction_product*>(data));},
            [](const void *data){tpInfo.write(static_cast<const map_transaction_product*>(data));},
            [](const std::function<void()> functionName){tpInfo.registerCallback(functionName);})
    },
    { DCInterface::transaction_privacy_info, 
        mappingInfo(
            [](void *data){tprivacyInfo.read(static_cast<map_transaction_privacy*>(data));},
            [](const void *data){tprivacyInfo.write(static_cast<const map_transaction_privacy*>(data));},
            [](const std::function<void()> functionName){tprivacyInfo.registerCallback(functionName);})
    },
    { DCInterface::user_id_info, 
        mappingInfo(
            [](void *data){gUserIds.read(static_cast<list_users *>(data));},
            [](const void *data){gUserIds.write(static_cast<const list_users*>(data));},
            [](const std::function<void()> functionName){gUserIds.registerCallback(functionName);})
    }
};

/// @brief static function to read data for the specific interface
/// @param interfaceName name of the interface
/// @param data pointer to copy the data to 
/// @return true if success else false
bool dataCollector::staticRead(DCInterface interfaceName, void* data){
    bool isItemPresent = (mappingTable.find(interfaceName) != mappingTable.cend());
    if(isItemPresent){
        auto info = mappingTable.at(interfaceName);
        info.mReadFunc(data);
    }
    return isItemPresent;
}

/// @brief static function to write data for the specific interface
/// @param interfaceName name of the interface
/// @param data pointer to copy the data from
/// @return true if success else false
bool dataCollector::staticWrite(DCInterface interfaceName, const void* data){
    bool isItemPresent = (mappingTable.find(interfaceName) != mappingTable.cend());
    if(isItemPresent){
        auto info = mappingTable.at(interfaceName);
        info.mWriteFunc(data);
    }
    return isItemPresent;
}

/// @brief static function to register callback when a write to interface is done
/// @param interfaceName name of the interface
/// @param functionName function object
/// @return true if success else false
bool dataCollector::staticRegisterCallback(DCInterface interfaceName, const std::function<void()> functionName){
    bool isItemPresent = (mappingTable.find(interfaceName) != mappingTable.cend());
    if(isItemPresent){
        auto info = mappingTable.at(interfaceName);
        info.mRegisterCallbackFunc(functionName);
    }
    return isItemPresent;
}

bool dataCollector::read(DCInterface interfaceName, void* data){
    return staticRead(interfaceName, data);
}

bool dataCollector::write(DCInterface interfaceName, const void* data){
    return staticWrite(interfaceName, data);
}

bool dataCollector::registerCallback(DCInterface interfaceName, const std::function<void()> functionName){
    return staticRegisterCallback(interfaceName, functionName);
}