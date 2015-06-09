<?php
namespace Oerp\Gallery\File\Uploader;

abstract class _Abstract {
  abstract protected function getAccount();
  abstract protected function upload($up);

  abstract protected function isAvailable();
  abstract protected function isOverLimit($up);
  abstract protected function isSkip($up);

  protected function getRootDir() {
    return drupal_get_path('module', 'oerp_gallery') . '/gallery';
  }

  protected function getQueueDir()
  {
    return $this->getRootDir() . '/to_' . $this->getAccount() . '_queue';
  }

  protected function getNewDir()
  {
    return $this->getRootDir() . '/to_' . $this->getAccount() . '_new';
  }

  protected function getGalleryDir()
  {
    return $this->getRootDir() . '/publish/' . $this->getAccount();
  }

  protected function updateQueue($text)
  {
    return file_put_contents($this->getQueueDir(), $text);
  }

  protected function updateNew($text){
    return file_put_contents($this->getNewDir(), $text);
  }

  protected function getUploads()
  {
    $queue = file_get_contents($this->getQueueDir());
    $new = file_get_contents($this->getNewDir());

    $ups = \Gulei\Text\Helper::explode($queue . "\n" . $new);
    $ups = array_unique($ups);
    return $ups;
  }

  protected function getLocalFile($up)
  {
    return $this->getGalleryDir() . '/' . $up;
  }

  protected function getLocalFileSize($up)
  {
    return filesize($this->getLocalFile($up));
  }

  protected function _exe()
  {
    $ups = $this->getUploads();
    $this->updateNew('');

    if (!$ups) {
      $this->updateQueue('');
      return NULL;
    }

    if(!$this->isAvailable()){
      $this->updateQueue(implode("\n", $ups));
      return false;
    }

    foreach($ups as $ind => $up){
      if ($this->isSkip($up)) {
        unset($ups[$ind]);
        continue;
      }

      if ($this->isOverLimit($up)) {
        break;
      }

      if($this->upload($up)){
        unset($ups[$ind]);
      }
    }

    if ($ups) {
      $this->updateQueue(implode("\n", $ups));
    }
    else {
      $this->updateQueue('');
    }
    return TRUE;
  }

  public function exe() {
    $rs = $this->_exe();
    $type = sprintf('Up.%s:', $this->getAccount());

    if (is_null($rs)) {
      watchdog($type, 'Idle.');
    }
    elseif ($rs) {
      watchdog($type, 'Executed.');
    }
    else {
      watchdog($type, 'Busy.');
    }
  }
}