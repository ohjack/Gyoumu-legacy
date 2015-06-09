<?php
namespace Oerp\Purchase\Hook\Form;

class FixPoState extends \Oerp\Purchase\Hook\Form\Abstract_PoController {
  const S_DONE = 'done';
  const S_WARN = 'approved';
  const S_WAIT = 'approved';
  const S_CANCEL = 'cancel';

  protected function _exe($param) {
    if ($this->rec['state'] != 'approved'
        && $this->rec['state'] != 'done'
    ) {
      return FALSE;
    }

    $picking = $this->getPicking();
    $is_cancel = TRUE;

    $status = self::S_DONE;
    $ts = date('Y-m-d H:i:s');

    foreach ($this->rec['order_line'] as $line) {
      $sku = $line['product_id']['default_code'];
      $overdue = strcmp($ts, $line['date_planned']) > 0;

      $got = $picking[$sku]['done'];
      $ordered = $line['product_qty'];

      if ($ordered > 0) {
        $is_cancel = FALSE;
      }

      if ($got < $ordered) {
        $status = self::S_WAIT;
        break;
      }

      if ($got > $ordered) {
        $status = self::S_WARN;
        break;
      }

      if ($got < $ordered && $overdue) {
        $status = self::S_WARN;
        break;
      }
    }

    if ($is_cancel) {
      $status = self::S_CANCEL;
    }

    $this->getQuery()->write(array(
      'state' => $status
    ));
  }
}