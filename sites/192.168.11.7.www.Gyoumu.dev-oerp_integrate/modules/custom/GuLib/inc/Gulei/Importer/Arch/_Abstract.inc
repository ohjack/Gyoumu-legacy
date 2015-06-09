<?php
namespace Gulei\Importer\Arch;

abstract class _Abstract implements \Gulei\Importer\_Interface
{
  protected function getBufferObj($form)
  {
    return $form['csv_file']['#obj']->renew();
  }
}