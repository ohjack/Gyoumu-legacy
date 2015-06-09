<?php
namespace Gulei;
require_once 'processor.inc';

interface InterfaceThemeable
{
  public function theme();
}

class ThemerTable implements InterfaceThemeable
{
  private $procHds;
  private $procRecs;

  public function __construct(\GuProcessor $procHds, \GuProcessor $procRecs)
  {
    $this->procHds = $procHds;
    $this->procRecs = $procRecs;
  }

  public function getProcHds()
  {
    return $this->procHds;
  }

  public function getProcRecs()
  {
    return $this->procRecs;
  }

  public function theme()
  {
    $procHds = clone $this->getProcHds();
    $procHds->addModifier(new ModTagize());

    $procRecs = clone $this->getProcRecs();
    $procRecs->addModifier(new ModTagize());

    return \theme('table', $procHds->getOutput(), $procRecs->getOutput());
  }
}

interface InterfaceIncluder
{
  static public function includeCss();
  static public function includeJs();
}

class IncluderTable implements InterfaceIncluder
{
  static public function includeCss()
  {
    drupal_add_css(drupal_get_path('module', 'GuLib') . '/css/table.css');
  }

  static public function includeJs()
  {
    drupal_add_js(drupal_get_path('module', 'GuLib') . '/js/GuTable.js');
  }
}