<?php
namespace Gulei\Importer\Arch;

abstract class _Abstract extends \Gulei\Importer\_Abstract
{
  protected function getBufferObj($form)
  {
    return $form['csv_file']['#obj']->renew();
  }

  protected function getRecs($form, $param = array())
  {
    $buf = $this->getBufferObj($form);
    return $buf->getRecordsAltered($param);
  }
}