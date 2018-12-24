#!/usr/bin/env bash

USER="sancho_test"
PASS="sancho_test"
DB="sancho_test_db"

sudo -u postgres dropdb --if-exists $DB
sudo -u postgres createdb $DB

sudo -u postgres dropuser --if-exists $USER
sudo -u postgres createuser $USER

sudo -u postgres psql -c "ALTER USER sancho_test WITH encrypted password '$PASS'"

PGPASSWORD=$PASS psql -U $USER -d $DB -h localhost -w -q -f ./setup.sql
