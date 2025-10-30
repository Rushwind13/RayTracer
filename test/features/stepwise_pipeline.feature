@skip
Feature: Stepwise pipeline smoke test (150x100)
  As a developer
  I want to run the pipeline step-by-step
  So that I can verify inputs/outputs at each bus and still produce an image

  Scenario: Stepwise 150x100 produces intermediate logs and final image
    Given I run the stepwise script with input "data/pixels_150x100.txt" as run "stepwise-150x100"

    # Stage 1: IntersectWith -> IntersectResult
    When the input file "data/pixels_150x100.txt" should exist with at least 15000 records
    Then the output file "runs/stepwise-150x100/oIntersectResult.txt" should exist with at least 15000 records

    # Stage 2: IntersectResults -> Shader
    When the input file "runs/stepwise-150x100/oIntersectResult.txt" should exist with at least 15000 records
    Then the output file "runs/stepwise-150x100/oShader.txt" should exist with at least 15000 records

    # Stage 3: Shader -> COLOR
    When the input file "runs/stepwise-150x100/oShader.txt" should exist with at least 15000 records
    Then the output file "runs/stepwise-150x100/oCOLOR.txt" should exist with at least 15000 records

    # Stage 4: ColorResults -> DEPTH
    When the input file "runs/stepwise-150x100/oCOLOR.txt" should exist with at least 15000 records
    Then the output file "runs/stepwise-150x100/oDEPTH.txt" should exist with at least 15000 records

    # Stage 5: DepthChart -> PNG bus
    When the input file "runs/stepwise-150x100/oDEPTH.txt" should exist with at least 15000 records
    Then the output file "runs/stepwise-150x100/oPNG.txt" should exist with at least 15000 records

    # Final sink: Writer produces actual PNG
    Then a PNG "bin/test.png" should exist
