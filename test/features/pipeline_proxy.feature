Feature: Proxy-based bus tests with ControlChannel
  As a developer
  I want to drive Feeder through the XPUB/XSUB proxy
  So that we validate ControlChannel usage and lack of leakage across runs

  Scenario: Feeder publishes via ControlChannel to Logger (no leakage single run)
    Given a fresh temp run directory
    And I bring all processes down
    And ControlChannel proxy on XSUB "tcp://127.0.0.1:1330" and XPUB "tcp://127.0.0.1:1331"
    And Logger on socket "tcp://127.0.0.1:1331" subscribing "ProxyTest" writing label "Proxy1.txt"
    When I feed 3 records from "test/fixtures/sample_3_records.txt" to socket "tcp://127.0.0.1:1330" on channel "ProxyTest"
    Then the logger artifact "Proxy1.txt" should have 3 records
  And I stop the proxy
  And I bring all processes down

  Scenario: ControlChannel does not leak between sequential runs
    Given a fresh temp run directory
    And I bring all processes down
    And ControlChannel proxy on XSUB "tcp://127.0.0.1:1332" and XPUB "tcp://127.0.0.1:1333"
    And Logger on socket "tcp://127.0.0.1:1333" subscribing "NoLeak" writing label "Leak1.txt"
    When I feed 3 records from "test/fixtures/sample_3_records.txt" to socket "tcp://127.0.0.1:1332" on channel "NoLeak"
    Then the logger artifact "Leak1.txt" should have 3 records
    And Logger on socket "tcp://127.0.0.1:1333" subscribing "NoLeak" writing label "Leak2.txt"
    When I feed 3 records from "test/fixtures/sample_3_records.txt" to socket "tcp://127.0.0.1:1332" on channel "NoLeak"
    Then the logger artifact "Leak2.txt" should have 3 records
  And I stop the proxy
  And I bring all processes down
