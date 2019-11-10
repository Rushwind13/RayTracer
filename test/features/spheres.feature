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
