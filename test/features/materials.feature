# language: en
Feature: Materials
  So that I can have objects control their color and material
  As a ray tracer
  I want to set and manipulate materials

  Scenario: the default material
    Given I have a material
    Then the result should be 1.0,0.1,0.1 a color
    And M ambient = 0.1
    And diffuse = 0.9
    And specular = 0.9
    And shininess = 200.0
