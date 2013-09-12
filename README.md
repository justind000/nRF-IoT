nRF-IoT
=======

RF24 based sensor-mesh (flood, addressless) network


*****This uses the modified RF24 library available here https://github.com/gcopeland/RF24. You'll need to replace your RF24 library with this one to compile.

It operates uses broadcasts and as such does not use or need any addresses. 

It is structured as follows:

[nodes >-> relay mesh >-> base]

Nodes only talk to relays, relays talk amongst each other, and also the talk to the base. 

The relays create a mesh network out of themselves. They broadcast every message they receive to every other relay. With this layout, only one relay has to be within range of the base, and each relay only needs to be within range of one other relay. The relays don't have addresses and broadcast blindly to each other. Logic is setup to prevent infinite rebroadcast loops between the relays. 

Nodes broadcast their messages to relays, but not to each other. If more than one relay can listen to a node, each relay will re-transmit that reading along to the base, creating a bit of redundancy. Nodes have delivery acknowledgment. 

The base potentially receives multiple copies of the same node message and only accepts the first instance of it. 

ADVANTAGES:
-simple to setup
-no addresses needed
-nodes can be eliminated in favor of an all relay network

DISADVANTAGES:
-relay messages flood the network. The number of messages (should be) (number of relays)^2. So with a large number of relays, a huge amount of traffic is generated. 
-The logic to stop infinite relay loops probably needs to be worked on more. It is very simple at this point and probably the most likely point of failure. 
-At this time, no acknowledgement of [relay >-> base]messages.
-At this time, one way communication only.
