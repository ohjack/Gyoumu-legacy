<?php
namespace Gulei\Importer\Arch;

abstract class _Abstract extends \Gulei\Importer\_Abstract
{
  protected function getBufferObj($form)
  {
    return $form['csv_file']['#obj']->renew();
  }
}