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

ActiveRecord::Schema.define(version: 20160221225154) do

  create_table "comments", force: true do |t|
    t.string   "author"
    t.text     "body"
    t.integer  "post_id"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  add_index "comments", ["post_id"], name: "index_comments_on_post_id"

  create_table "posts", force: true do |t|
    t.string   "title"
    t.text     "body"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "author"
  end

  create_table "users", force: true do |t|
    t.string   "name"
    t.string   "password_digest"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

end





class CategoryController < ApplicationController
    def checked
        @user.category.nil? ? false: @user.category.match(area)
   end
 end
        
#model
class Category
  include Mongoid::Document
  field :type, type: String
  field :category, type: Array, default: []

  belongs_to :user
end
        
def read_forms
    @category = Category.new(category_params)

    if @category.save
        redirect_to action: 'suggestions'
    else
        render action: 'new'
    end
end

private
        def category_params
        params.require(:category).permit(:type, category: [])
end



require 'sqlite3'
module CategoryHelper

def display_filtered_clubs
	list_of_categories = Arrays.new
	list_of_categories << read_forms

	list_of_categories.each do | filter |
		print_database

end
    
def read_forms 

end
    
    

def print_database
	begin
        db = SQLite3::Database.open "allclubs.db"
        
        #goes through each element of the array and then inputs that into the query
        list_of_categories.each do |item|
            stm = db.execute "SELECT club_name FROM allclubs WHERE category = #{item} "
            #sets the parameter
            stm.bind_param 1, item
            rs = stm.execute

            rs.each do |row|
                printf "%s\n", row['club_name']
            end
        end
    
        rescue SQLite3::Exception => e 
    
            puts "Exception occurred"
            puts e
    
        ensure
            db.close if db
    end

end