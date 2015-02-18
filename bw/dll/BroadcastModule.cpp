#include "BroadcastModule.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "bw_generated.h"

using namespace BWAPI;
using namespace Filter;
using namespace bw;
using namespace flatbuffers;
namespace fb = flatbuffers;

BroadcastModule::BroadcastModule() : context(1), publisher(context, ZMQ_PUB), worker(context, ZMQ_REQ)
{
  publisher.bind("tcp://*:5556");
  worker.bind("tcp://localhost:5555");
}

void BroadcastModule::onStart()
{
  Broodwar->pauseGame();
}

void BroadcastModule::onEnd(bool isWinner)
{
}

void BroadcastModule::onFrame()
{

  if (Broodwar->isPaused()) {
    zmq_pollitem_t items [] = { { worker,  0, ZMQ_POLLIN, 0 } };

    int rc = zmq_poll (items, 1, 100);

    return;
  }

  static int frame_count = 0;
  Broodwar->drawTextScreen(200,  0, "FPS: %d", Broodwar->getFPS());
  Broodwar->drawTextScreen(200, 20, "onFrame: %d", frame_count);

  FlatBufferBuilder fbb;
  std::vector<fb::Offset<bw::Player>> players;
  for (auto player : Broodwar->getPlayers()) {
    auto name = fbb.CreateString(player->getName());
    auto p = CreatePlayer(fbb, player->getID(), name);
    players.push_back(p);
  }
  auto v = fbb.CreateVector(players);
  auto game = CreateGame(fbb, v);
  auto change_set = CreateChangeSet(fbb, game, 0);
  FinishChangeSetBuffer(fbb, change_set);
  zmq::message_t message(fbb.GetSize());
  Broodwar << "Frame size" << fbb.GetSize() << std::endl;
  memcpy(message.data(), fbb.GetBufferPointer(), fbb.GetSize());
//  publisher.send("BWAI", 4, ZMQ_MORE);
  publisher.send(message);
}
