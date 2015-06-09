<?php
namespace Fedex\Provider;

class PrintOverview implements \Gulei\Provider\_Interface
{
  public function getData()
  {
    $sql = '
      SELECT * FROM {fedprint_recs}
      WHERE status > 0
      ORDER BY rid DESC
    ';

    $rs = pager_query(
      $sql, 100, 0,
      'SELECT COUNT(*) FROM {fedprint_recs} WHERE status > 0'
    );

    $print_recs = array();
    while ($data = db_fetch_object($rs)) {
      $print_recs[] = $data;
    }

    $ccode = _fedprint_getCountryCodeOpts();

    $recs = array();
    foreach ($print_recs as $print_rec) {
      $rec = array();
      $src = unserialize($print_rec->src);

      $rec['rid'] = $print_rec->rid;
      $rec['tid'] = $src['tid'];
      $rec['ref'] = $src['ref_note'];
      $rec['name'] = $src['recp_contact'];
      $rec['country'] = $ccode[$src['recp_country_code']];

      $resp = fedprint_check_print($print_rec->rid);
      $rec['date'] = $resp['date'];
      $rec['status'] = $resp['msg'];

      $recs[] = $rec;
    }

    $errors = array();
    foreach ($recs as $rec) {
      if (!$rec['date']) {
        $errors[] = $rec;
      }
    }

    $recs = array_filter(
      $recs,
      create_function(
        '$i', 'return !empty($i["date"]);'
      )
    );

    $recs = array_merge($errors, $recs);
    return $recs;
  }

  public function count()
  {
      $sql = <<<SQL
  SELECT COUNT(*) FROM {fedprint_recs}
  WHERE status > 0
SQL;

    return db_result(db_query($sql));
  }

  public function getReference()
  {
    return null;
  }
}