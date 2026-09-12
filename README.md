# LVP
Live Video Production

LVP is a framework to allow video content producers create live video productions in real time using simple node based controls and dynamic timeline control.

## !!Important!!

The initial build for LVP will be Windows only!

I will not support platforms right now, the aim is to get a working version running asap, and cross platform is _not_ a priority.

## LVP Core

The main components of the framework are:
- Input Handlers (source videos, images, audio, documents and custom inputs - like webcams etc).
- Input Filters (various graphics and audio filters to apply to input sources).
- Script Processor (allow control, annotation, stepping, skipping and the usual timeline interations that can be controlled by script)
- Device Controller (interface with US devices to then operate inputs, filters and scripts)
- Output Handlers (Output to OBS - plugin, direct to streams, files, ip/network and media routers)

The LVP Core is a set of libraries that LVP bundles and manages.

## LVP UI

The LVP UI is based around lightweight common tools. Initial trials are using imgui and nuklear.
A nodes based UI is in development for simplified runtime use.

- Custom UI widgets shall be able to be developed.
- Nodes shall map to the core objects as well as a series of overlay widgets that can be added to the output or purely just for runtime use.
- The UI shall allow exporting to other video toolkits (with limited support)
- Projects can be saved and modified
- Projects can use widget and layout templates to allow for complex realtime control setups
The UI is both an offline (pre-production) and realtime toolkit.

## LVP DB

The database backend shall provide:
- Revisioning
- Backup
- User control/security
- Sharing and multi-user content generation
- Duplication for realtime stability

The databases shall be local, and completely user controlled. 

## Future

Being a reasonably substantial project the planning is based around an initial full pipeline test.
The test shall progress:
1. Initial basic core elements.
2. Initial base UI.
3. Initial base DB

The test shall specifically be aimed at proving the realtime editing and production output to a live video stream.
