<?php
namespace Chte\Processor;

abstract class _Abstract
{
  protected $input;
  protected $defaults;
  protected $headers;

  abstract public function getRecords();

  function __construct($input, $defaults, $headers)
  {
    $this->input = $input;
    $this->defaults = $defaults;
    $this->headers = $headers;
  }
}