<?php
namespace Yauc\Query;

abstract class _Abstract
{
  const AUC_NEW =  1;
  const AUC_LOADED = 2;
  const AUC_SOLD = 3;
  const AUC_CLOSE = 4;
  const AUC_ERROR = 5;
  const AUC_SOLD_IGNORE = 6;
  const AUC_CLOSE_IGNORE = 7;
  const AUC_DEAD = 8;

  const table_name = 'yaucapi_auction';
}
