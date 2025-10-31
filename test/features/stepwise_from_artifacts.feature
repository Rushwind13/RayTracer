@skip
Feature: Stepwise pipeline from artifacts (resume Stage 2–6)
  As a developer
  I want to resume the stepwise pipeline from existing artifacts
  So that I can verify Stage 2–6 chaining and render PNGs deterministically

  # Uses defaults from runs/complete for missing inputs; focuses on producing o*.txt outputs
  Scenario: Resume from Stage 2 using runs/complete artifacts
    Given I run the stepwise script resuming from stage "2" as run "stepwise-from-complete-stage2"
    Then the output file "runs/stepwise-from-complete-stage2/oCOLOR.txt" should exist with at least 100 records
    Then the output file "runs/stepwise-from-complete-stage2/oDEPTH.txt" should exist with at least 100 records
    Then the output file "runs/stepwise-from-complete-stage2/oPNG.txt" should exist with at least 100 records

  Scenario: Resume from Stage 3 using runs/complete artifacts
    Given I run the stepwise script resuming from stage "3" as run "stepwise-from-complete-stage3"
    Then the output file "runs/stepwise-from-complete-stage3/oCOLOR.txt" should exist with at least 100 records
    Then the output file "runs/stepwise-from-complete-stage3/oDEPTH.txt" should exist with at least 100 records
    Then the output file "runs/stepwise-from-complete-stage3/oPNG.txt" should exist with at least 100 records

  Scenario: Resume from Stage 4 using runs/complete artifacts
    Given I run the stepwise script resuming from stage "4" as run "stepwise-from-complete-stage4"
    Then the output file "runs/stepwise-from-complete-stage4/oDEPTH.txt" should exist with at least 100 records
    Then the output file "runs/stepwise-from-complete-stage4/oPNG.txt" should exist with at least 100 records

  Scenario: Resume from Stage 5 using runs/complete artifacts
    Given I run the stepwise script resuming from stage "5" as run "stepwise-from-complete-stage5"
    Then the output file "runs/stepwise-from-complete-stage5/oPNG.txt" should exist with at least 100 records

  # PNG rendering directly from artifacts via Writer
  Scenario: Render PNG from existing oPNG artifact
    Given I render a PNG from artifact "runs/complete/stages/oPNG.txt" to "bin/test-from-artifacts.png"
    Then a PNG "bin/test-from-artifacts.png" should exist
