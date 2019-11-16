# language: en
Feature: Shapes
  So that I can test the Object base class
  As a Ray Tracer
  I want to create and test a TestObject class

## Materials
  Scenario: A shape has a default material
    Given I have a shape
    And I have a material
    Then the shape's material is the default

  Scenario: A shape may be assigned a material
    Given I have a shape
    And I have a material
    When I set m.ambient = 1
    And I set the shape's material
    Then O ambient = 1

  Scenario: Computing the normal on a translated shape
    Given I have a shape
    And I have a position 2,3,4 in the data
    And I create a translation matrix A
    When I set the shape's transform to A
    Then MO<0,3> = 2
    And  MO<1,3> = 3
    And  MO<2,3> = 4
    And  MO<3,3> = 1
