Feature: Writer OID precedence and EOF stop
  As a developer
  I want Writer to prefer Intersection OID over Pixel OID and stop on EOF
  So that coloring is consistent and pipeline termination is correct

  Scenario: OID precedence rule (Intersection over Pixel)
    Given pixel oid 5 and intersection oid 7
    Then the chosen oid should be 7
