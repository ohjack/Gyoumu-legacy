<?php
namespace Gulei\Importer;

interface _Interface
{
  public function getArch();
  public function submit($form, &$form_state);
}