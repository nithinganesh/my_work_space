#include <bits/stdc++.h>

using map_transaction_product = std::unordered_map<std::string, std::string>;
using map_transaction_privacy = std::unordered_map<std::string, std::string>;
using list_users = std::unordered_set<std::string>;

enum class DCInterface : size_t{
    transaction_product_info,
    transaction_privacy_info,
    user_id_info
};

class dataCollectorIf{
public:

    virtual bool read(DCInterface interfaceName, void* data) =0;
    virtual bool write(DCInterface interfaceName, const void* data) =0;
    virtual bool registerCallback(DCInterface interfaceName, const std::function<void()> functionName) =0;
};

class dataCollector : public dataCollectorIf{

private:
    static bool staticRead(DCInterface interfaceName, void* data);
    static bool staticWrite(DCInterface interfaceName, const void* data);
    static bool staticRegisterCallback(DCInterface interfaceName, const std::function<void()> functionName);

public:

    bool read(DCInterface interfaceName, void* data) override;
    bool write(DCInterface interfaceName, const void* data) override;
    bool registerCallback(DCInterface interfaceName, const std::function<void()> functionName) override;
};