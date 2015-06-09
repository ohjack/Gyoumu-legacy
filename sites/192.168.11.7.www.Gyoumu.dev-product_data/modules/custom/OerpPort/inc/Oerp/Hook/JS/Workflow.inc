<?php
namespace Oerp\Hook\JS;

class Workflow extends \Oerp\Hook\JS\_Abstract {
  public function getWeight() {
    return self::WT_WORKFLOW;
  }

  public function applicable() {
    return $this->post['exec'] == 'exec_workflow';
  }

  public function exe() {
    $resp = \Oerp\Query\Basic::_execute(
      $this->send,
      'exec_workflow'
    );
    return drupal_json($resp);
  }
}