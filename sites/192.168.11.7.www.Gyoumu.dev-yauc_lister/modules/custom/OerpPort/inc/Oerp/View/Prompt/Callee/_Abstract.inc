<?php
namespace Oerp\View\Prompt\Callee;

abstract class _Abstract
{
  protected $post;

  abstract public function execute();
  abstract public function isApplicable();

  public function __construct($post)
  {
    $this->post = $post;
  }
}