# Core Module Communication — Initial Design Brief

## Purpose

The system will consist of a collection of independent executable modules that communicate with one another in realtime.

The initial target platform is **Windows 11 x64**.

The longer-term goal is to provide the communication and execution foundation for a realtime content-processing system supporting video, audio, images, text, streaming data, and related content-creator workflows.

## Core Architectural Principle

Every process in the system is a peer from the perspective of the underlying communication protocol.

Infrastructure services such as Atlas, Zeus, Apollo and Hermes do **not** have a special UDP protocol.

A Dummy module, an infrastructure service, or a future video-processing module all use exactly the same:

- UDP transport
- Common envelope
- Packet format
- Addressing model
- Serialization/deserialization
- Receive semantics
- Send semantics

The difference between processes is entirely in their application-level behaviour.

## Communication

Modules will communicate using **UDP**.

UDP provides the underlying physical network addressing:

- Source IP / port
- Destination IP / port

The application protocol will **not duplicate UDP source/destination addressing**.

A module therefore does not configure a target IP or target port.

A module primarily needs to know the local UDP port on which it listens.

## Common Protocol

All processes use a common, module-agnostic message protocol.

The protocol consists of two conceptual layers:

```text
UDP Datagram
    |
    +-- Common Envelope
    |
    +-- Payload
```

### Common Envelope

The envelope will remain intentionally small and descriptive.

Initial fields:

- **UID** — unique identifier for the message.
- **Module Owner** — logical identity of the module/system that owns or originated the message.
- **Module Target** — logical identity of the intended module/consumer.
- **Memory Scope** — logical namespace defining the scope/context in which the message exists.
- **Timestamp** — time associated with the message.

Additional envelope metadata may be introduced as requirements emerge.

The envelope should **not contain payload-specific information**.

The envelope should contain enough information to support future routing, discovery and resolution mechanisms, potentially using a **DNS-like logical naming model**.

The exact logical naming scheme is intentionally not yet defined.

### Payload

The payload is separate from the common envelope and is opaque to the communication layer.

A payload may represent any type of data, including:

- Video
- Audio
- Images
- Text
- Control data
- Streaming datasets
- Module-specific data
- Discovery information
- Notifications

The common protocol must not assume a particular payload type.

Individual processes are responsible for understanding and processing payloads they support.

## Module Behaviour

A module listens for incoming protocol messages.

When a message is received, the module inspects the common envelope and determines whether it can process the message.

Conceptually:

```text
Receive packet
      |
      v
Inspect common envelope
      |
      +-- Can process --> Process payload
      |
      +-- Cannot process -> Ignore
```

A module may generate output in response to an incoming packet.

The response is itself a normal protocol packet and uses the same UDP/common-envelope/payload structure.

The module does not have a preconfigured target for normal communication. The destination of a response is determined from the communication context and/or routing information associated with the packet.

## Discovery

A DNS-like discovery service will provide local module discovery and logical-to-physical endpoint resolution.

The discovery service is named **Atlas**.

Atlas maintains a registry of active processes/modules and their UDP endpoints.

Modules announce their creation to Atlas and announce their destruction/withdrawal when shutting down.

Conceptually:

```text
Module starts
     |
     v
MODULE_ANNOUNCE
     |
     v
Atlas registry
     |
     v
Notify interested modules

Similarly:

```text
Module shuts down
|
v
MODULE_WITHDRAW
|
v
Atlas registry
|
v
Notify interested modules

Discovery traffic uses the **same common protocol** as all other traffic.

There is no separate discovery transport or privileged discovery protocol.

Potential initial discovery messages include:

- `MODULE_ANNOUNCE`
- `MODULE_WITHDRAW`
- `MODULE_DISCOVERY_REQUEST`
- `MODULE_DISCOVERY_RESPONSE`
- Module availability/change notifications

The exact message types and payload definitions remain to be determined.

## System Services — Pantheon

The system will use a Greek mythology naming theme for major infrastructure processes.

### Atlas — Discovery

Atlas provides the DNS-like discovery and local registry service.

Responsibilities include:

- Maintaining module identity → UDP endpoint mappings
- Receiving module announcements
- Receiving module withdrawal notifications
- Notifying modules of discovery/lifecycle events
- Supporting future logical routing and resolution

Atlas is not privileged at the protocol level.

### Zeus — Process Control

Zeus is the master process-control service.

Potential responsibilities include:

- Starting processes
- Stopping processes
- Monitoring processes
- Detecting process failure
- Restart/recovery
- External system control
- Future administrative functions

Zeus communicates using the exact same UDP/protocol interface as every other process.

### Apollo — Data / Configuration / Stores

Apollo provides shared data and state infrastructure.

Potential responsibilities include:

- Configuration
- Shared memory/data
- Data stores
- Persistent storage
- Realtime shared state

Apollo communicates using the exact same UDP/protocol interface as every other process.

### Hermes — Messaging / Notifications

Hermes provides higher-level messaging and notification services.

Potential responsibilities include:

- Notifications
- Event distribution
- Pub/sub
- Subscriptions
- Higher-level message routing

Hermes communicates using the exact same UDP/protocol interface as every other process.

### Dummy — Test Module

Dummy is intentionally simple and exists to validate the architecture.

Dummy has no special privileges.

It communicates using the exact same UDP/protocol interface as Atlas, Zeus, Apollo, Hermes and all future modules.

## Process Equality

From the UDP/protocol perspective:

```text
Atlas.exe
Zeus.exe
Apollo.exe
Hermes.exe
Dummy.exe
VideoModule.exe
AudioModule.exe
FutureModule.exe
        |
        v
    SAME UDP
        |
        v
 SAME COMMON PROTOCOL
        |
        v
 SAME ENVELOPE
        |
        v
     PAYLOAD
```

Infrastructure services are distinguished by their application behaviour, not by special transport mechanisms.

For example:

```text
Atlas receives packet
    -> interprets payload as discovery information

Zeus receives packet
    -> interprets payload as process-control information

Apollo receives packet
    -> interprets payload as data/store information

Hermes receives packet
    -> interprets payload as messaging/notification information

Dummy receives packet
    -> performs test behaviour
```

All of these packets remain protocol-equivalent.

## Routing

Physical addressing is provided by UDP:

```text
UDP source IP:port
UDP destination IP:port
```

Logical addressing is provided by the common envelope:

```text
Module Owner
Module Target
Memory Scope
```

Atlas will eventually provide resolution between logical module identities and physical UDP endpoints.

This allows logical module identities to remain independent of physical network topology.

## Response-Based Communication

Modules do not maintain arbitrary configured target endpoints for normal application communication.

Communication is generally driven by received packets.

A module may consume an incoming packet and produce a response packet.

Conceptually:

```text
Incoming packet
      |
      v
Module
      |
      +-- consume
      |
      +-- process
      |
      +-- generate response
                    |
                    v
              Outgoing packet
```

For early tests, fixed localhost UDP endpoints may be used to create simple communication paths.

These fixed endpoints are test infrastructure and are not part of the module's logical addressing model.

## Security and Authority

A security and authorization layer will likely be introduced in a future phase.

Security does not change the fundamental protocol equality between processes.

In particular:

**Zeus having administrative authority must not require Zeus to use a different communication protocol.**

Authority should instead be established through mechanisms such as:

- Authentication
- Identity
- Authorization
- Permissions/policies
- Cryptographic credentials

Conceptually:

```text
Common Protocol
      |
      v
Security / Identity
      |
      v
Authentication
      |
      v
Authorization
      |
      v
Allowed operation
```

Zeus may eventually administer system authority and policy, but this is an application/security concern rather than a transport distinction.

Security architecture is intentionally deferred at this stage.

