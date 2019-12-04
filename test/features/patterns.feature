# language: en
Feature: Patterns
  So that I can use patterns on my objects,
  As a Ray tracer,
  I want to do lots of pattern manipulation

  Background:
    Given Black is the color 0.1,0.1,0.1
    And White is the color 1.0,1.0,1.0

  Scenario: Creating a stripe pattern
    Given I have a stripe pattern
    Then pattern.a = white
    And pattern.b = black

  Scenario: A stripe pattern is constant in Y
    Given I have a stripe pattern
    Then stripe_at 0,0,0 is white
    And stripe_at 0,1,0 is white
    And stripe_at 0,2,0 is white

  Scenario: A stripe pattern is constant in Z
    Given I have a stripe pattern
    Then stripe_at 0,0,0 is white
    And stripe_at 0,0,1 is white
    And stripe_at 0,0,2 is white

  Scenario: A stripe pattern is alternates in X
    Given I have a stripe pattern
    Then stripe_at 0,0,0 is white
    And stripe_at 0.9,0,0 is white
    And stripe_at 1,0,0 is black
    And stripe_at -0.1,0,0 is black
    And stripe_at -1,0,0 is black
    And stripe_at -1.1,0,0 is white

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


