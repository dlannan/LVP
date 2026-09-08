## Initial Implementation

The first implementation will consist of two simple processes:

1. **Atlas** — minimal discovery service.
2. **Dummy** — minimal test module.

The initial objective is to validate the common communication architecture rather than implement useful application functionality.

The first test should demonstrate:

1. Atlas starts and listens on a known UDP port.
2. Dummy starts and listens on its own UDP port.
3. Dummy announces itself to Atlas.
4. Atlas receives and validates the announcement.
5. Atlas records Dummy's logical identity and UDP endpoint.
6. Atlas notifies Dummy of the discovery event.
7. Dummy receives and processes the notification.
8. Dummy shuts down.
9. Dummy sends a withdrawal notification.
10. Atlas removes Dummy from its registry.
11. Atlas can notify other interested modules of the withdrawal.

Once this works, two or more Dummy instances can be used to validate discovery between independent processes.

## Initial Test Topology

A simple localhost topology is sufficient initially:

```text
                 Atlas
               UDP :4000
                   |
          Discovery traffic
                   |
          ┌────────┴────────┐
          │                 │
          ▼                 ▼
      Dummy A           Dummy B
     UDP :5001          UDP :5002
```

The ports are test configuration only.

The logical identities and packet envelopes remain independent of these physical endpoints.

## Guiding Principles

- **UDP is the transport.**
- **UDP owns physical network addressing.**
- **Every process uses the same UDP/protocol interface.**
- **No process is privileged at the transport/protocol level.**
- **The common envelope owns logical identity and context.**
- **The payload is independent of the communication protocol.**
- **The envelope should remain minimal.**
- **New envelope fields should be added only when a real requirement exists.**
- **Modules should not contain hard-coded application targets.**
- **Communication should generally be response/event driven.**
- **Logical identity should not be tightly coupled to physical network location.**
- **Discovery is a service, not a special transport.**
- **Infrastructure processes are ordinary protocol participants.**
- **Authority is an application/security concern, not a protocol distinction.**
- **The protocol should support the eventual realtime media system without prematurely encoding media-specific assumptions.**
- **Early implementations should favour simple executable tests over premature abstraction.**

## Development Sequence

The anticipated infrastructure development sequence is:

```text
1. Dummy <-> Dummy
       |
       v
2. Atlas <-> Dummy
       |
       v
3. Atlas + Multiple Dummies
       |
       v
4. Zeus process management
       |
       v
5. Apollo shared data/configuration
       |
       v
6. Hermes messaging/notifications
       |
       v
7. Security / authority
       |
       v
8. Actual media/content modules
```

Each stage should validate the underlying architecture before introducing the next layer of complexity.

