# language: en
Feature: Spheres
  In order to render Spheres
  As a ray tracer
  I want to do lots of math with Spheres

  Scenario: A ray intersects a sphere at two points
      Given I have a Ray with origin 0,0,-5 and direction 0,0,1
      And I have a sphere
      When I intersect the ray with the sphere
      Then an intersection occurred
      #And intersection count = 2
      And intersection distance 0 = 4.0
      #And intersection distance 1 = 6.0

  Scenario: A ray intersects a sphere at a tangent
      Given I have a Ray with origin 0,1,-5 and direction 0,0,1
      And I have a sphere
      When I intersect the ray with the sphere
      Then an intersection occurred
      #And intersection count = 2
      And intersection distance 0 = 5.0
      #And intersection distance 1 = 6.0

  Scenario: A ray misses a sphere
      Given I have a Ray with origin 0,2,-5 and direction 0,0,1
      And I have a sphere
      When I intersect the ray with the sphere
      Then no intersection occurred

  Scenario: A ray originates inside a sphere
      Given I have a Ray with origin 0,0,1 and direction 0,0,1
      And I have a sphere
      When I intersect the ray with the sphere
      Then no intersection occurred

  Scenario: A sphere is behind a ray
      Given I have a Ray with origin 0,0,5 and direction 0,0,1
      And I have a sphere
      When I intersect the ray with the sphere
      Then no intersection occurred
## Sphere Normals
  Scenario: The normal on a sphere at a point on the X axis
      Given I have a sphere
      When I calculate the normal at 1,0,0
      Then the result should be 1,0,0 a vector

  Scenario: The normal on a sphere at a point on the Y axis
      Given I have a sphere
      When I calculate the normal at 0,1,0
      Then the result should be 0,1,0 a vector

  Scenario: The normal on a sphere at a point on the Z axis
      Given I have a sphere
      When I calculate the normal at 0,0,1
      Then the result should be 0,0,1 a vector

  Scenario: The normal on a sphere at a non-axial point
      Given I have a sphere
      When I calculate the normal at 0.57735,0.57735,0.57735
      Then the result should be 0.57735,0.57735,0.57735 a vector

  Scenario: The normal is a normalized vector
      Given I have a sphere
      When I calculate the normal at 0.57735,0.57735,0.57735
      Then the result should be 0.57735,0.57735,0.57735 a vector
      And the vector is normalized
## Normals transforms
  Scenario: Computing the normal on a translated sphere
    Given I have a sphere
    And I have a position 0,1,0 in the data
    And I create a translation matrix A
    When I set the sphere's transform to A
    And I calculate the normal at 0,1.70711,-0.70711
    Then the result should be 0,0.70711,-0.70711 a vector

  Scenario: Computing the normal on a scaled sphere
    Given I have a sphere
    And I have a position 2,2,2 in the data
    And I create a scaling matrix A
    When I set the sphere's transform to A
    And I calculate the normal at 0,1.70711,-0.70711
    Then the result should be 0,0.923879,-0.382684 a vector

  Scenario: Computing the normal on a transformed sphere
    Given I have a sphere
    And I have a position 1,0.5,1 in the data
    And I create a scaling matrix A
    And I create a rotation matrix B 36 degrees about 0,0,1
    When R = A * B
    And I set the sphere's transform to R
    And I calculate the normal at 0,0.70711,-0.70711
    Then the result should be 0,0.97014,-0.24254 a vector
## Materials
  Scenario: A sphere has a default material
    Given I have a sphere
    And I have a material
    Then the sphere's material is the default

  Scenario: A sphere may be assigned a material
    Given I have a sphere
    And I have a material
    When I set m.ambient = 1
    And I set the sphere's material
    Then S ambient = 1
