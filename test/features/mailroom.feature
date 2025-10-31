@skip
Feature: Mailroom routes IntersectResults to Shader/Background/Black/Lit
  As a pipeline integrator
  I want a simple divide-out actor
  So that an aggregated IntersectResults stream can be split into four routed outputs

  Background:
    Given a fresh temp run directory

  # This scenario uses an existing aggregated sample (to be provided) containing
  # mixed primary hits/misses and shadow cases. It validates that Mailroom
  # produces four routed artifacts obeying the 2N-line invariant.
  Scenario: Route mixed IntersectResults records
    Given Mailroom on socket "tcp://127.0.0.1:1312" subscribing "IntersectResults" publishing on socket "tcp://127.0.0.1:1312"
    And Logger on socket "tcp://127.0.0.1:1312" subscribing "Shader" writing label "Shader.txt"
    And Logger on socket "tcp://127.0.0.1:1312" subscribing "Background" writing label "BKG.txt"
    And Logger on socket "tcp://127.0.0.1:1312" subscribing "Black" writing label "BLACK.txt"
    And Logger on socket "tcp://127.0.0.1:1312" subscribing "Lit" writing label "LIT.txt"
  When I feed 3 records from "test/fixtures/sample_intersectresults_mixed.txt" to socket "tcp://127.0.0.1:1312" on channel "IntersectResults"
  Then the logger artifact "Shader.txt" should have 2 records
  And the logger artifact "BKG.txt" should have 1 records
  And the logger artifact "BLACK.txt" should have 0 records
  And the logger artifact "LIT.txt" should have 0 records
