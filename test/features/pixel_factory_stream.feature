Feature: PixelFactory streaming yields blank payload lines in Logger
  As a developer
  I want PixelFactory to publish Pixel-only messages
  So that Logger writes blank Intersection lines in the artifact

  Scenario: PixelFactory → Logger writes blank payload lines
    Given a fresh temp run directory
    And Logger on socket "tcp://127.0.0.1:1322" subscribing "PTest" writing label "PFStream.txt"
    And PixelFactory streaming 5 records on socket "tcp://127.0.0.1:1322" channel "PTest"
    Then the logger artifact "PFStream.txt" should have 5 records
    And the logger artifact "PFStream.txt" should have 5 blank payload lines
