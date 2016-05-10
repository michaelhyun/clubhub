# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

<<<<<<< HEAD
ActiveRecord::Schema.define(version: 20160510210938) do
=======
ActiveRecord::Schema.define(version: 20160510060008) do
>>>>>>> f2a25440105d9447d71f7bc42a21cdb31f5a5fdc

  create_table "categories", force: true do |t|
    t.string   "genre"
    t.string   "clubs"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "clubs", force: true do |t|
    t.string   "name"
    t.string   "admin"
    t.string   "numberOfMembers"
    t.text     "description"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "category_id"
    t.integer  "rating"
    t.text     "reviews"
  end

  add_index "clubs", ["category_id"], name: "index_clubs_on_category_id"

  create_table "reviews", force: true do |t|
    t.string   "rating"
    t.text     "comment"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "review_id"
    t.integer  "club_id"
  end

  create_table "users", force: true do |t|
    t.string   "name"
    t.string   "password_digest"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "username"
    t.string   "email"
    t.string   "salt"
  end

end
