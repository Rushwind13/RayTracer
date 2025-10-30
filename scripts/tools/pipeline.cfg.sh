#!/usr/bin/env bash
# Central configuration for pipeline topics and sockets

# Proxy (XSUB for publishers, XPUB for subscribers)
PROXY_XSUB="tcp://127.0.0.1:1314"
PROXY_XPUB="tcp://127.0.0.1:1313"

# Buses (direct PUB/SUB sockets used by services outside the proxy)
BUS_INTERSECT_RESULT="tcp://127.0.0.1:1300" # IntersectResults bind/subscribes here in continuous mode
BUS_SHADER="tcp://127.0.0.1:1312"
BUS_COLOR="tcp://127.0.0.1:1303"
BUS_DEPTH="tcp://127.0.0.1:1307"
BUS_PNG="tcp://127.0.0.1:1308"

# Topics
TOPIC_INTERSECT_RESULT="IntersectResult"
TOPIC_SHADER="Shader"
TOPIC_COLOR="COLOR"
TOPIC_DEPTH="DEPTH"
TOPIC_PNG="PNG"

# World objects for IntersectWith fan-out
OBJECTS=(
  box1
  sphere2
  sphere3
  plane_floor
  plane_l_wall
  plane_r_wall
)

# Default bind/connect roles for stepwise runs (overridable per invocation)
# Convention: Feeder always connects. For two-party flows, make the consumer bind its SUB.
# Components read these via environment variables in their binaries.
export SHADER_BIND_SUB_DEFAULT=1
export COLORRESULTS_BIND_SUB_DEFAULT=1
export DEPTHCHART_BIND_SUB_DEFAULT=1
export WRITER_BIND_SUB_DEFAULT=1
export LOGGER_BIND_DEFAULT=0
