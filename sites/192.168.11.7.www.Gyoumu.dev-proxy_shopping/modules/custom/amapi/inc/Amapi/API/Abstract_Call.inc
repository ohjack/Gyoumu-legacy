<?php
namespace Amapi\API;

abstract class Abstract_Call extends Abstract_Config
{
  abstract function getResponse($args = null);
}