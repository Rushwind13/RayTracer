# language: en
Feature: Boxes
  So that I can use boxes in my scenes
  As a Ray Tracer
  I want to do lots of math with boxes

Scenario: A ray intersects a box +X
  Given I have a box
  And I have a Ray with origin 5,0.5,0 and direction -1,0,0
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box -X
  Given I have a box
  And I have a Ray with origin -5,0.5,0 and direction 1,0,0
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box +Y
  Given I have a box
  And I have a Ray with origin 0.5,5,0 and direction 0,-1,0
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box -Y
  Given I have a box
  And I have a Ray with origin 0.5,-5,0 and direction 0,1,0
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box +Z
  Given I have a box
  And I have a Ray with origin 0.5,0,5 and direction 0,0,-1
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box -Z
  Given I have a box
  And I have a Ray with origin 0.5,0,-5 and direction 0,0,1
  When I intersect the ray with the box
  Then an intersection occurred
  #And intersection count = 2
  And intersection distance 0 = 4
  #And intersection distance 1 = 6

Scenario: A ray intersects a box inside
  Given I have a box
  And I have a Ray with origin 0,0.5,0 and direction 0,0,1
  When I intersect the ray with the box
  Then no intersection occurred
  #And intersection count = 2
  # And intersection distance 0 = -1
  #And intersection distance 1 = 1

Scenario: A ray misses a box
  Given I have a box
  And I have a Ray with origin -2,0,0 and direction 0.2673,0.5345,0.8018
  When I intersect the ray with the box
  Then no intersection occurred

Scenario: A ray misses a box
  Given I have a box
  And I have a Ray with origin 0,-2,0 and direction 0.8018,0.2673,0.5345
  When I intersect the ray with the box
  Then no intersection occurred

Scenario: A ray misses a box
  Given I have a box
  And I have a Ray with origin 0,0,-2 and direction 0.5345,0.8018,0.2673
  When I intersect the ray with the box
  Then no intersection occurred

Scenario: A ray misses a box
  Given I have a box
  And I have a Ray with origin 2,0,2 and direction 0,0,-1
  When I intersect the ray with the box
  Then no intersection occurred

Scenario: A ray misses a box
  Given I have a box
  And I have a Ray with origin 0,2,2 and direction 0,-1,0
  When I intersect the ray with the box
  Then no intersection occurred

Scenario: A ray misses a box
  Given I have a box
  And I have a Ray with origin 2,2,0 and direction -1,0,0
  When I intersect the ray with the box
  Then no intersection occurred


Scenario: A ray misses a box
  Given I have a box
  And I have a Ray with origin 2,2,0 and direction -1,-0.1,0
  When I intersect the ray with the box
  Then no intersection occurred
#### Normals
  Scenario: The normal on the surface of a box
    Given I have a box
    When I calculate the box normal at 1,0.5,-0.8
    Then the result should be 1,0,0 a vector
    When I calculate the box normal at -1,-0.2,0.9
    Then the result should be -1,0,0 a vector
    When I calculate the box normal at -0.4,1,-0.1
    Then the result should be 0,1,0 a vector
    When I calculate the box normal at 0.3,-1,-0.7
    Then the result should be 0,-1,0 a vector
    When I calculate the box normal at -0.6,0.3,1
    Then the result should be 0,0,1 a vector
    When I calculate the box normal at 0.4,0.4,-1
    Then the result should be 0,0,-1 a vector
    When I calculate the box normal at 1,1,1
    Then the result should be 1,0,0 a vector
    When I calculate the box normal at -1,-1,-1
    Then the result should be -1,0,0 a vector
