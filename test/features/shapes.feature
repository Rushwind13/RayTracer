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

  Scenario: A shape can be assigned a transform
    Given I have a shape
    And I have a position 2,3,4 in the data
    And I create a translation matrix A
    When I set the shape's transform to A
    Then MO<0,3> = 2
    And  MO<1,3> = 3
    And  MO<2,3> = 4
    And  MO<3,3> = 1
## Ray intersections
  Scenario: Intersecting a scaled shape with a ray
    Given I have a Ray with origin 0,0,-5 and direction 0,0,1
    Given I have a shape
    And I have a position 2,2,2 in the data
    And I create a scaling matrix A
    When I set the shape's transform to A
    When I intersect the ray with the shape
    Then the origin should be 0,0,-2.5 a position
    And the direction should be 0,0,0.5 a vector

  Scenario: Intersecting a translated shape with a ray
    Given I have a Ray with origin 0,0,-5 and direction 0,0,1
    Given I have a shape
    And I have a position 5,0,0 in the data
    And I create a translation matrix A
    When I set the shape's transform to A
    When I intersect the ray with the shape
    Then the origin should be -5,0,-5 a position
    And the direction should be 0,0,1 a vector
## Normals transforms
  Scenario: Computing the normal on a translated shape
    Given I have a shape
    And I have a position 0,1,0 in the data
    And I create a translation matrix A
    When I set the shape's transform to A
    And I calculate the shape normal at 0,1.70711,-0.70711
    Then the result should be 0,0.70711,-0.70711 a vector

  Scenario: Computing the normal on a transformed shape
    Given I have a shape
    And I have a position 1,0.5,1 in the data
    And I create a scaling matrix A
    And I create a rotation matrix B 36 degrees about 0,0,1
    When R = A * B
    And I set the shape's transform to R
    And I calculate the shape normal at 0,0.70711,-0.70711
    Then the result should be 0,0.97014,-0.24254 a vector
