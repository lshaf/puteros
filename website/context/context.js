"use client";
const type = {
  MODAL: "MODAL",
  PORTFOLIOMODAL: "PORTFOLIOMODAL",
  BLOGMODAL: "BLOGMODAL",
  GALLERY: "GALLERY",
};
const { MODAL, PORTFOLIOMODAL, BLOGMODAL, GALLERY } = type;

import { createContext, useCallback, useReducer } from "react";

const context = createContext();

const reducer = (state, action) => {
  const { type, payload } = action;
  switch (type) {
    case MODAL:
      return {
        ...state,
        modal: payload,
      };
    case PORTFOLIOMODAL:
      return {
        ...state,
        portfolioModal: payload,
      };
    case BLOGMODAL:
      return {
        ...state,
        blogModal: payload,
      };
    case GALLERY:
      return {
        ...state,
        gallery: payload,
      };
    default:
      return state;
  }
};

const state = (props) => {
  const initialState = {
    modal: false,
    portfolioModal: null,
    blogModal: null,
    gallery: false,
  };
  const [state, dispatch] = useReducer(reducer, initialState);

  const modalToggle = useCallback((value) => {
    dispatch({
      type: MODAL,
      payload: value,
    });
  }, []);

  const setPortfolioModal = useCallback((value) => {
    dispatch({
      type: PORTFOLIOMODAL,
      payload: value,
    });
  }, []);
  const setBlogModal = useCallback((value) => {
    dispatch({
      type: BLOGMODAL,
      payload: value,
    });
  }, []);
  const setGallery = useCallback((value) => {
    console.log(value);
    dispatch({
      type: GALLERY,
      payload: value,
    });
  }, []);

  const { modal, portfolioModal, blogModal, gallery } = state;
  return (
    <context.Provider
      value={{
        modal,
        modalToggle,
        portfolioModal,
        setPortfolioModal,
        blogModal,
        setBlogModal,
        gallery,
        setGallery,
      }}
    >
      {props.children}
    </context.Provider>
  );
};

export default state;

export { context };
