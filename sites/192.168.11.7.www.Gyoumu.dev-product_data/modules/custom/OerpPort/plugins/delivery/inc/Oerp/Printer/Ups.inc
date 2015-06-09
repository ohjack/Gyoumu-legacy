<?php
namespace Oerp\Printer;

class Ups extends \Ups\Printer
{
  function __construct($id)
  {
    parent::__construct(
      '/home/jeremy/UpsListener/in',
      '/home/jeremy/UpsListener/out',
      $id
    );
  }
}
