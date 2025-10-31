Feature: 150x2 cross-section renders end-to-end
  As a developer
  I want to render a 150x2 slice (rows 49-50) from a canonical interleaved artifact
  So that the pipeline demonstrates end-to-end behavior without editing artifacts

  Scenario: Render 150x2 slice via Feeder→Writer
    Given a fresh temp run directory
    And a 150x2 slice artifact from "data/pixels_150x100.txt" rows 49-50
  And Writer bound on socket "tcp://127.0.0.1:1324" output "../bin/slice_150x2.png"
  When I feed 300 records from "test/tmp/run/slice.txt" to socket "tcp://127.0.0.1:1324" on channel "PNG"
    Then PNG "bin/slice_150x2.png" should exist
