# cod
##### *Cryptographically Obfuscated D(istributed hash table)*

**cod** aims to implement a distributed hash table with lots of spooky crypto thrown in.
Using the term *cryptographically obfuscated* as no claims are being made as to the security and because it spells a fish.

## Process

 1. **codd** establishes itself on a random port and *tries* to find peers
     - No idea how this bootstrapping will work if everyone's on a random port
     - Could use DNS but then who maintains the DNS?
     - If network agrees that an agent has sufficient reputation let it add itself to DNS?
     - Additionally, as port numbers are changeable, need to use TXT records for ports
 2. **codd** communicates with peers to `ESTABLISH` its identity on the network
     - Initially, **codd** is communicating its own `nodeID`, public key and routing information
     - At this point, doesn't have much trust on the network as there's only one source of its publickey
     - Probably going to get a lot of challenges...
 3. 

Before transmission, messages:
 - Are signed with sender's key
 - Encrypted with AES-256 using a random key and IV
 - Have the AES key and IV attached using recipient's public key

## Message types
 - FIND
 - SET
 - KNOW
 - ESTABLISH
 - CHALLENGE
 - RESPONSE
 - ACK
 - NACK
