Feature: Integration: Feeder↔Logger and Feeder→Writer
  As a developer
  I want to exercise real actors with a tiny feed
  So that CSV artifacts and PNG output are produced end-to-end

  Scenario: Feeder publishes to Logger on a test socket
    Given a fresh temp run directory
    And Logger on socket "tcp://127.0.0.1:1320" subscribing "ITest" writing label "ITest.txt"
    When I feed 5 records from "data/pixels.txt" to socket "tcp://127.0.0.1:1320" on channel "ITest"
    Then the logger artifact "ITest.txt" should have 5 records

  Scenario: Feeder drives Writer to produce a PNG
    Given Writer bound on socket "tcp://127.0.0.1:1321" output "../bin/test_int.png"
    When I feed 5 records from "data/pixels.txt" to socket "tcp://127.0.0.1:1321" on channel "PNG"
    Then PNG "bin/test_int.png" should exist
