<?php
namespace Gulei\Modifier\Tag;

abstract class _Abstract extends \Gulei\Modifier\_Abstract
{
  function __construct($keys = null)
  {
    parent::__construct(null, 'data', $keys);
  }
}