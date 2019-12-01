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

