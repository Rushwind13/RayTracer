Feature: Run a single stage in isolation (only_one_up)
  As a developer
  I want to run an individual pipeline stage using only_one_up
  So that I can validate stage I/O and produce the expected artifact quickly

  # Stage 4: ColorResults -> DEPTH from existing COLOR
  Scenario: Stage 4 produces oDEPTH from oCOLOR
    Given I run only-one-up stage "4" with input "runs/complete/stages/oCOLOR.txt" as run "oneup-stage4"
    Then the output file "runs/oneup-stage4/oDEPTH.txt" should exist with at least 50 records

  # Stage 5: DepthChart -> PNG channel from existing DEPTH
  Scenario: Stage 5 produces oPNG from oDEPTH
    Given I run only-one-up stage "4" with input "runs/complete/stages/oCOLOR.txt" as run "oneup-stage5-prep"
    And I run only-one-up stage "5" with input "runs/oneup-stage5-prep/oDEPTH.txt" as run "oneup-stage5"
    Then the output file "runs/oneup-stage5/oPNG.txt" should exist with at least 50 records

  # Stage 6: Writer -> PNG file from existing oPNG channel
  Scenario: Stage 6 produces PNG from oPNG
    Given I run only-one-up stage "4" with input "runs/complete/stages/oCOLOR.txt" as run "oneup-stage6-prep4"
    And I run only-one-up stage "5" with input "runs/oneup-stage6-prep4/oDEPTH.txt" as run "oneup-stage6-prep5"
    And I run only-one-up stage "6" with input "runs/oneup-stage6-prep5/oPNG.txt" as run "oneup-stage6"
    Then a PNG "runs/oneup-stage6/test.png" should exist
