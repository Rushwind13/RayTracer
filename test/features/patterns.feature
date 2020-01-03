# language: en
Feature: Patterns
  So that I can use patterns on my objects,
  As a Ray tracer,
  I want to do lots of pattern manipulation

  Background:
    Given Black is the color 0.0,0.0,0.0
    And White is the color 1.0,1.0,1.0

  Scenario: Creating a stripe pattern
    Given I have a stripe pattern
    Then pattern.a = white
    And pattern.b = black

  Scenario: A stripe pattern is constant in Y
    Given I have a stripe pattern
    Then S pattern_at 0,0,0 is 1,1,1
    And S pattern_at 0,1,0 is 1,1,1
    And S pattern_at 0,2,0 is 1,1,1

  Scenario: A stripe pattern is constant in Z
    Given I have a stripe pattern
    Then S pattern_at 0,0,0 is 1,1,1
    And S pattern_at 0,0,1 is 1,1,1
    And S pattern_at 0,0,2 is 1,1,1

  Scenario: A stripe pattern is alternates in X
    Given I have a stripe pattern
    Then S pattern_at 0,0,0 is 1,1,1
    And S pattern_at 0.9,0,0 is 1,1,1
    And S pattern_at 1,0,0 is 0,0,0
    And S pattern_at -0.1,0,0 is 0,0,0
    And S pattern_at -1,0,0 is 0,0,0
    And S pattern_at -1.1,0,0 is 1,1,1

  Scenario: Stripes with an object transformation
    Given I have a shape
    And I have a position 2,2,2 in the data
    And I create a scaling matrix A
    And I have a stripe pattern
    When I set the shape's transform to A
    And I set the shape's pattern to stripe
    Then stripe_at_shape 1.5,0,0 is white
    And pointer cleanup occurred

  Scenario: Stripes with a pattern transformation
    Given I have a shape
    And I have a position 2,2,2 in the data
    And I create a scaling matrix A
    And I have a stripe pattern
    When I set the stripe's transform to A
    And I set the shape's pattern to stripe
    Then stripe_at_shape 1.5,0,0 is white
    And pointer cleanup occurred

  Scenario: Stripes with both an object and a pattern transformation
    Given I have a shape
    And I have a stripe pattern
    And I have a position 2,2,2 in the data
    And I create a scaling matrix A
    And I have a position 0.5,0,0 in the data
    And I create a translation matrix B
    When I set the shape's transform to A
    And I set the stripe's transform to B
    And I set the shape's pattern to stripe
    Then stripe_at_shape 2.5,0,0 is white
    And pointer cleanup occurred
### Pattern Transforms
    Scenario: The default pattern transformation
        Given I have a test pattern
        Then pattern.transform = I

    Scenario: Assigning a transformation
        Given I have a test pattern
        And I have a position 1,2,3 in the data
        And I create a translation matrix A
        When I set the pattern's transform to A
        Then pattern.transform = translation<1,2,3>

    Scenario: Patterns with an object transformation
        Given I have a shape
        And I have a position 2,2,2 in the data
        And I create a scaling matrix A
        And I have a test pattern
        When I set the shape's transform to A
        And I set the shape's pattern to pattern
        Then pattern_at_shape 2,3,4 is 1,1.5,2
        And pointer cleanup occurred

    Scenario: Patterns with a pattern transformation
        Given I have a shape
        And I have a position 2,2,2 in the data
        And I create a scaling matrix A
        And I have a test pattern
        When I set the pattern's transform to A
        And I set the shape's pattern to pattern
        Then pattern_at_shape 2,3,4 is 1,1.5,2
        And pointer cleanup occurred

    Scenario: Patterns with both an object and a pattern transformation
        Given I have a shape
        And I have a test pattern
        And I have a position 2,2,2 in the data
        And I create a scaling matrix A
        And I have a position 0.5,1,1.5 in the data
        And I create a translation matrix B
        When I set the shape's transform to A
        And I set the pattern's transform to B
        And I set the shape's pattern to pattern
        Then pattern_at_shape 2.5,3,3.5 is 0.75,0.5,0.25
        And pointer cleanup occurred

### Gradient, Ring
    Scenario: A gradient linearly interpolates between colors
        Given I have a gradient pattern
        Then G pattern_at 0,0,0 is 1,1,1
        Then G pattern_at 0.25,0,0 is 0.75,0.75,0.75
        Then G pattern_at 0.5,0,0 is 0.5,0.5,0.5
        Then G pattern_at 0.75,0,0 is 0.25,0.25,0.25

    Scenario: A ring should extend in both X and Z
        Given I have a ring pattern
        Then R pattern_at 0,0,0 is 1,1,1
        Then R pattern_at 1,0,0 is 0,0,0
        Then R pattern_at 0,0,1 is 0,0,0
        Then R pattern_at 0.708,0,0.708 is 0,0,0
