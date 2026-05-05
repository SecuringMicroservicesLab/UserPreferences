#include <iostream>
#include <string>
#include <map>
#include <grpcpp/grpcpp.h>
#include "../services.grpc.pb.h"

#include "../include/laserpants/dotenv/dotenv.h"

using namespace grpc;
using namespace secure_lab;

struct PreferenceData
{
  std::string color;
  int32_t number;
};

class PreferencesServiceImpl final : public PreferencesService::Service
{
private:
  // map name to their stats
  std::map<std::string, PreferenceData> user_db_;

public:
  // construct the data
  PreferencesServiceImpl() 
  {
    user_db_["Owen"] = {"Orange", 41};
    user_db_["Chris"] = {"Blue", 42};
    user_db_["Akash"] = {"Green", 67};

    std::cout << "[Preferences] Database loaded with " << user_db_.size() << " profiles." << std::endl;
  }

  // The RPC function called by the Orchestrator
  Status GetUserStats(ServerContext* context, const Name* nameRequest, UserPreferences* preferencesReply) override 
  {
    std::string incoming_name = nameRequest->name();
    std::cout << "[Preferences] Fetching data for: " << incoming_name << "... ";

    // Search the map for the incoming name
    auto it = user_db_.find(incoming_name);

    // If 'it' doesn't equal the end of the map, the key exists
    if (it != user_db_.end()) 
    {
      // it->second accesses the PreferenceData struct attached to the name
      preferencesReply->set_fav_color(it->second.color);
      preferencesReply->set_fav_number(it->second.number);
      std::cout << "FOUND." << std::endl;
    }
    else
    {
      // this will likely never happen if the two services have the same data
      preferencesReply->set_fav_color("Unknown");
      preferencesReply->set_fav_number(-1);
      std::cout << "NOT FOUND." << std::endl;
    }

    // The network transmission succeeded, so return OK
    return Status::OK;
  }
};

int main()
{
  dotenv::init();

  PreferencesServiceImpl service;
  ServerBuilder builder;

  // Fallback to 0.0.0.0:50052 if not found
  std::string server_address = dotenv::getenv("PREFERENCES_ADDRESS", "0.0.0.0:50052");
  
  builder.AddListeningPort(server_address, InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "[Preferences] Server listening on " << server_address << std::endl;

  server->Wait();
  return 0;
}
