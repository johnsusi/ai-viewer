#pragma once
#include <BWAPI.h>
#include <zmq.hpp>

// Remember not to use "Broodwar" in any global class constructor!

class BroadcastModule : public BWAPI::AIModule
{
public:
	BroadcastModule();
  void onStart() override;
  void onEnd(bool isWinner) override;
  void onFrame() override;
private:
	zmq::context_t context;
	zmq::socket_t publisher;

};
