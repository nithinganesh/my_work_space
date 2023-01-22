#include "dataCollector.h"

void test_users(){
    list_users users{"user1","user2","user3"};
    std::shared_ptr<dataCollectorIf> dataCollectorPtr = std::make_shared<dataCollector>();
    dataCollectorPtr->write(DCInterface::user_id_info,&users);
}

void test_transactions(){
    map_transaction_product data;
    std::shared_ptr<dataCollectorIf> dataCollectorPtr = std::make_shared<dataCollector>();
    dataCollectorPtr->read(DCInterface::transaction_product_info, &data);

    for(auto const &[transaction, product]: data){
        std::cout << transaction << " : " << product << std::endl;
    }
    data.clear();
    data["transaction_1"]="product_3";
    dataCollectorPtr->write(DCInterface::transaction_product_info, &data);
    map_transaction_product read_data;
    dataCollectorPtr->read(DCInterface::transaction_product_info, &read_data);
    assert(read_data==data);
}

void usersRegisteredCallback(){
    list_users read_users;
    std::shared_ptr<dataCollectorIf> dataCollectorPtr = std::make_shared<dataCollector>();
    dataCollectorPtr->read(DCInterface::user_id_info,&read_users);
    std::cout << "List of users got updated. Reading new list" << std::endl;
    for(auto const&user: read_users){
        std::cout << user << " , ";
    }
    std::cout << std::endl;
}

int main ()
{
    std::shared_ptr<dataCollectorIf> dataCollectorPtr = std::make_shared<dataCollector>();
    dataCollectorPtr->registerCallback(DCInterface::user_id_info, usersRegisteredCallback);
    test_users();
    return 0;
}