nRF-IoT
=======

RF24 based sensor-mesh (flood, addressless) network


*****This uses the modified RF24 library available here https://github.com/gcopeland/RF24. You'll need to replace your RF24 library with this one to compile (this fork adds broadcast capability, and also claims minor performance improvements, various bug fixes, not exactly drop-in compatible so be warned).

Most of this was implemented using the original RF24 driver available here http://maniacbug.github.io/RF24/

Some ideas and code were also taken from this project https://github.com/mic159/ArduinoMesh

It operates using broadcasts and as such does not use or need any addresses. 

To get this working, the most straight-forward setup is with three boards. Burn node, relay, and base to each and they should all start talking to each other. Other possible setups would be two or more relays, or relays and a base. There is some testing code in the relays to send out a broadcast (look at the bottom for Serial.read), negating the need for a node. 

It is structured as follows:

[nodes >-> relay mesh >-> base]

Nodes only talk to relays, relays talk amongst each other, and also the talk to the base. 

The relays create a mesh network amongst themselves. They broadcast every message they receive to every other relay. With this layout, only one relay has to be within range of the base, and each relay only needs to be within range of one other relay. The relays don't have addresses and broadcast blindly to each other. Logic is setup to prevent infinite rebroadcast loops between the relays. 

Nodes broadcast their messages to relays, but not to each other. If more than one relay can listen to a node, each relay will re-transmit that reading along to the base, creating a bit of redundancy. Nodes have delivery acknowledgment. 

The base potentially receives multiple copies of the same node message and only accepts the first instance of it. 

ADVANTAGES:

-simple to setup.

-no addresses needed.

-nodes can be eliminated in favor of an all relay network.

-small, under 8k. Could get smaller with some tinkering (remove printf from RF24)

DISADVANTAGES:

-relay messages flood the network. The number of messages (should be) (number of relays)^2. So with a large number of relays, a huge amount of traffic is generated. 

-The logic to stop infinite relay loops probably needs to be worked on more. It is very simple at this point and probably the most likely point of failure. 

-At this time, no acknowledgement of [relay >-> base]messages.

-At this time, one way communication only.

I have included a python version of the base sketch. I run this as a daemon on a beaglebone black and it serves as my base. The python base also uploads the data to a self-hosted emoncms site. It makes some slight changes, namely dynamic payloads don't seem to work in this setup, and the address of the base is changed.
