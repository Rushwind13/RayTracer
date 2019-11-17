# language: en
Feature: Shapes
  So that I can use Planes in my world
  As a Ray Tracer
  I want to create and test a Plane class

  Scenario: The normal of a plane is constant everywhere
    Given I have a plane
    When I calculate the plane normal at 0,0,0
    Then the result should be 0,1,0 a vector
    When I calculate the plane normal at 10,0,-10
    Then the result should be 0,1,0 a vector
    When I calculate the plane normal at -5,0,150
    Then the result should be 0,1,0 a vector

  Scenario: Intersect with a ray parallel to the plane
    Given I have a plane
    Given I have a Ray with origin 0,10,0 and direction 0,0,1
    When I intersect the ray with the plane
    Then no intersection occurred

  Scenario: Intersect with a coplanar ray
    Given I have a plane
    Given I have a Ray with origin 0,0,0 and direction 0,0,1
    When I intersect the ray with the plane
    Then no intersection occurred

  Scenario: A ray intersecting a plane from above
    Given I have a plane
    Given I have a Ray with origin 0,1,0 and direction 0,-1,0
    When I intersect the ray with the plane
    Then an intersection occurred
    And intersection distance 0 = 1.0
    And intersection object = P
